#pragma once
#include "safe_queue.hxx"
#include <condition_variable>
#include <functional>
#include <future>
#include <map>
#include <mutex>
#include <optional>
#include <thread>
#include <type_traits>

namespace kangsw {
class timeout_exception : public std::exception {
public:
    explicit timeout_exception(char const* _Message)
        : exception(_Message)
    {
    }
};

template <typename Ty_>
class future_proxy {
    friend class thread_pool;

public:
    std::future<Ty_> get_future()
    {
        auto future = promise_->get_future();
        promise_.reset();
        return std::move(future);
    }

    future_proxy() = default;
    future_proxy(const future_proxy& other) = delete;
    future_proxy(future_proxy&& other) noexcept = default;
    future_proxy& operator=(const future_proxy& other) = delete;
    future_proxy& operator=(future_proxy&& other) noexcept = default;

private:
    std::shared_ptr<std::promise<Ty_>> promise_;
};

template <>
class future_proxy<void> {
};

class thread_pool {
public:
    using clock = std::chrono::system_clock;
    using task_function = std::function<void()>;

public:
    thread_pool(size_t task_queue_cap_ = 1024, size_t num_workers = std::thread::hardware_concurrency(), size_t concrete_worker_count_limit = 1024) noexcept;
    ~thread_pool();

    thread_pool(const thread_pool& other) = delete;
    thread_pool(thread_pool&& other) noexcept = delete;
    thread_pool& operator=(const thread_pool& other) = delete;
    thread_pool& operator=(thread_pool&& other) noexcept = delete;

public:
    void resize_worker_pool(size_t new_size);
    size_t num_workers() const { return num_workers_cached_; }
    size_t num_pending_task() const { return tasks_.size(); }
    size_t task_queue_capacity() const { return tasks_.capacity(); }
    size_t num_available_workers() const { return num_workers_cached_ - num_working_workers_; }
    clock::duration average_wait() const { return clock::duration(average_wait_.load()); }
    size_t num_max_workers() const { return num_max_workers_; }
    void num_max_workers(size_t value);

    template <typename Fn_, typename... Args_>
    decltype(auto) launch_task(Fn_&& f, Args_... args);

protected:
    template <typename Fn_, typename... Args_>
    static decltype(auto) _package_task(
      thread_pool::task_function& event, Fn_&& f, Args_... args);

private:
    bool _try_add_worker();
    void _pop_workers(size_t count);
    void _check_reserve_worker(size_t threshold);

public:
    std::chrono::milliseconds launch_timeout_ms{1000};
    std::chrono::microseconds max_stall_interval_time{20000};
    std::chrono::microseconds max_task_wait_time{5000};

private:
    struct worker_t {
        struct atomic_bool_wrap_t {
            std::atomic_bool value;

            atomic_bool_wrap_t() = default;
            atomic_bool_wrap_t(const atomic_bool_wrap_t& other) = delete;
            atomic_bool_wrap_t(atomic_bool_wrap_t&& other) noexcept
            {
                value = other.value.load();
            }
            atomic_bool_wrap_t& operator=(const atomic_bool_wrap_t& other) = delete;
            atomic_bool_wrap_t& operator=(atomic_bool_wrap_t&& other) noexcept
            {
                value = other.value.load();
                return *this;
            }
        };
        std::thread thread;
        atomic_bool_wrap_t disposer;
    };

    struct task_t {
        task_function event;
    };

private:
    safe_queue<task_t> tasks_;
    std::vector<worker_t> workers_;
    mutable std::mutex worker_lock_;

    std::condition_variable event_wait_;
    mutable std::mutex event_lock_;

    std::atomic_size_t num_workers_cached_;
    std::atomic_size_t num_working_workers_;
    std::atomic_size_t num_max_workers_;

    std::atomic<clock::time_point> latest_active_ = clock::now();
    std::atomic<clock::time_point> latest_event_;
    std::atomic<int64_t> average_wait_;
};

template <typename Fn_, typename... Args_> decltype(auto)
thread_pool::_package_task(thread_pool::task_function& event, Fn_&& f, Args_... args)
{
    using callable_return_type = std::invoke_result_t<Fn_, Args_...>;
    using return_type = future_proxy<callable_return_type>;
    using returns_void = std::is_same<callable_return_type, void>;
    using promise_ptr = std::shared_ptr<std::promise<callable_return_type>>;
    return_type result;

    if constexpr (returns_void::value) {
        event = [fn_ = std::forward<Fn_>(f),
                 arg_tuple_ = std::make_tuple(std::forward<Args_>(args)...)]() mutable {
            // Storing exception features are only available on release build,
            //to improve debug
            if constexpr (returns_void::value) {
                std::apply(fn_, std::move(arg_tuple_));
            }
        };
    }
    else {
        result.promise_ = std::make_shared<std::promise<callable_return_type>>();
        event = [promise = result.promise_,
                 fn_ = std::forward<Fn_>(f),
                 arg_tuple_ = std::make_tuple(std::forward<Args_>(args)...)]() mutable {
            try {
                promise->set_value(std::apply(fn_, std::move(arg_tuple_)));
            } catch (std::exception&) {
                do {
                    try {
                        // if future was already retrieved, below statement will throw.
                        // then do nothing, to mandate error handling to caller
                        auto v = promise->get_future();
                    } catch (std::exception&) {
                        break;
                    }

                    // else, given promise was expired before the future was retrieved.
                    // then this block simply rethrow the exception, which occurs program termination.
                    throw;
                } while (false);

                // if above statement did not thrown, it means the future was correctly
                //retrieved when it was launched.
                promise->set_exception(std::current_exception());
            }
        };
    }

    return result;
}

template <typename Fn_, typename... Args_>
decltype(auto) thread_pool::launch_task(Fn_&& f, Args_... args)
{
    static_assert(std::is_invocable_v<Fn_, Args_...>);

    using callable_return_type = std::invoke_result_t<Fn_, Args_...>;
    using return_type = future_proxy<callable_return_type>;
    using returns_void = std::is_same<callable_return_type, void>;
    using promise_ptr = std::shared_ptr<std::promise<callable_return_type>>;

    return_type result;

    using std::chrono::system_clock;

    _check_reserve_worker(1);

    task_t task;
    result = _package_task<Fn_, Args_...>(task.event, std::forward<Fn_>(f), std::forward<Args_>(args)...);

    latest_event_ = clock::now();
    for (auto elapse_begin = system_clock::now();
         !tasks_.try_push(std::move(task));
         std::this_thread::yield()) {
        if (system_clock::now() - elapse_begin > launch_timeout_ms) {
            throw timeout_exception{""};
        }
    }

    event_wait_.notify_one();
    return result;
}

inline thread_pool::thread_pool(size_t task_queue_cap_, size_t num_workers, size_t worker_limit) noexcept
    : tasks_(task_queue_cap_)
    , num_max_workers_(worker_limit)
{
    resize_worker_pool(num_workers);
}

inline thread_pool::~thread_pool()
{
    _pop_workers(workers_.size());
}

inline void thread_pool::resize_worker_pool(size_t new_size)
{
    new_size = std::min(num_max_workers_.load(), new_size);
    if (new_size == 0) {
        throw std::invalid_argument{"Size 0 is not allowed"};
    }

    std::lock_guard<std::mutex> lock(worker_lock_);
    if (new_size > workers_.size()) {
        while (new_size != workers_.size()) {
            _try_add_worker();
        }
    }
    else if (new_size < workers_.size()) {
        _pop_workers(workers_.size() - new_size);
    }
}

inline void thread_pool::num_max_workers(size_t value)
{
    if (value == 0) {
        throw std::invalid_argument("0 is not allowed");
    }

    std::lock_guard<std::mutex> lock{worker_lock_};
    num_max_workers_ = value;

    if (value < workers_.size()) {
        _pop_workers(workers_.size() - value);
    }
}

inline bool thread_pool::_try_add_worker()
{
    if (workers_.size() >= num_max_workers_) {
        return false;
    }

    auto worker = [this, index = workers_.size()]() {
        task_t task;

        while (workers_[index].disposer.value == false) {
            if (tasks_.try_pop(task)) {
                auto now = clock::now();

                if (num_available_workers() == 1) {
                    auto wait_time = (now - latest_event_.load()).count();
                    auto average = average_wait_.load();

                    auto new_average = ((average * 7) + wait_time) / 8;
                    auto diff = new_average - average;
                    average_wait_.fetch_add(diff);
                }

                _check_reserve_worker(1);
                latest_active_ = now;
                latest_event_ = now;
                num_working_workers_.fetch_add(1);

                task.event();
                num_working_workers_.fetch_sub(1);
            }
            else {
                std::unique_lock<std::mutex> lock(event_lock_);
                event_wait_.wait(lock);
            }
        }
    };

    auto& wd = workers_.emplace_back();
    wd.disposer.value = false;
    wd.thread = std::thread(std::move(worker));

    num_workers_cached_ = workers_.size();
    return true;
}

inline void thread_pool::_pop_workers(size_t count)
{
    auto const begin = workers_.end() - count;
    auto const end = workers_.end();

    for (auto it = begin; it != end; ++it) {
        it->disposer.value.store(true);
    }
    event_wait_.notify_all();
    for (auto it = begin; it != end; ++it) {
        it->thread.join();
    }

    workers_.erase(begin, end);
    num_workers_cached_ = workers_.size();
}

inline void thread_pool::_check_reserve_worker(size_t threshold)
{
    if ( // reserve workers if required.
      num_available_workers() <= threshold
      && (clock::now() - latest_active_.load() > max_stall_interval_time
          || average_wait() > max_task_wait_time)) {
        resize_worker_pool((num_workers() & ~1) + 2);
    }
}

// timer thread pool
class timer_thread_pool : public thread_pool {
public:
    timer_thread_pool(
      size_t task_queue_cap_ = 1024,
      size_t num_workers = std::thread::hardware_concurrency(),
      size_t concrete_worker_count_limit = -1)
        : thread_pool(task_queue_cap_, num_workers, concrete_worker_count_limit)
    {
        timer_thread_ = std::thread{[this]() {
            while (!pending_dispose_.load()) {
                if (std::unique_lock lock{timer_lock_}) {
                    timer_thread_wait_.wait_until(lock, nearlest_awake_.load());
                }

                if (nearlest_awake_.load() > clock::now()) {
                    // Yet awake time is far ...
                    continue;
                }

                std::unique_lock lock(timer_lock_);
                while (!pending_timers_.empty()
                       && clock::now() > pending_timers_.begin()->first) {
                    // since 'pending_timers_' is always sorted by ascending order,
                    //frontmost element is always the first pending timer node.
                    launch_task(std::move(pending_timers_.begin()->second));
                    pending_timers_.erase(pending_timers_.begin());
                }

                // checks 'pending_timer_' during the lock is alive.
                if (pending_timers_.empty()) {
                    nearlest_awake_.store(clock::time_point::max(), std::memory_order_relaxed);
                }
                else {
                    nearlest_awake_.store(pending_timers_.begin()->first, std::memory_order_relaxed);
                }

                num_waiting_timer_.store(pending_timers_.size(), std::memory_order_relaxed);
            }
        }};
    }

    ~timer_thread_pool()
    {
        pending_dispose_.store(true);
        timer_thread_wait_.notify_one();
        timer_thread_.join();
    }

public:
    template <typename Fn_, typename... Args_>
    decltype(auto) launch_timer(std::chrono::microseconds delay, Fn_&& f, Args_... args)
    {
        task_function event;
        auto result = _package_task(event, std::forward<Fn_>(f), std::forward<Args_>(args)...);

        std::unique_lock lock(timer_lock_);
        auto issue = clock::now() + delay;

        nearlest_awake_.store(issue);
        pending_timers_.try_emplace(issue, std::move(event));
        timer_thread_wait_.notify_one();

        num_waiting_timer_.store(pending_timers_.size(), std::memory_order_relaxed);
        return result;
    }

public:
    size_t num_total_waitings() const { return num_waiting_timer_.load() + num_pending_task(); }
    size_t num_waiting_timer() const { return num_waiting_timer_.load(); }

private:
    std::thread timer_thread_;
    std::atomic_bool pending_dispose_;

    std::atomic<clock::time_point> nearlest_awake_ = clock::time_point::max();
    std::map<clock::time_point, std::function<void()>> pending_timers_;
    std::condition_variable timer_thread_wait_;
    std::atomic_size_t num_waiting_timer_;
    mutable std::mutex timer_lock_;
};
} // namespace kangsw
