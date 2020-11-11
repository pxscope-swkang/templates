#pragma once
#include <atomic>
#include <list>
#include <memory>
#include <mutex>
#include <shared_mutex>

namespace kangsw {
namespace LOCK_FREE_CIRCULAR_DEPRECATED__ {
template <typename Ty_>
class safe_queue {
public:
    using element_type = Ty_;

public:
    safe_queue(size_t capacity)
        : array_elem_(new element_type[capacity + 1])
        , array_occupied_(new std::atomic_bool[capacity + 1])
        , capacity_(capacity + 1)
    {
        memset(array_occupied_.get(), 0, capacity + 1);
    }

    template <typename RTy_>
    bool try_push(RTy_&& elem)
    {
        size_t tail = tail_.load();
        size_t next = (tail + 1) % capacity_;
        if (next == head()) { return false; }

        if (tail_.compare_exchange_weak(tail, next)) {
            // tail is unique here.
            // wait until other thread's reading head consumes current slot.
            while (array_occupied_[tail]) {}

            array_elem_[tail] = std::forward<RTy_>(elem);
            array_occupied_[tail].store(true);
            return true;
        }
        return false;
    }

    bool try_pop(Ty_& retval)
    {
        size_t head = head_.load();
        size_t next = (head + 1) % capacity_;
        if (head == tail()) { return false; }

        if (head_.compare_exchange_weak(head, next)) {
            // head is unique here.
            // wait until other thread's writing tail fills current slot
            while (!array_occupied_[head]) {}

            retval = std::move(array_elem_[head]);
            array_occupied_[head].store(false); // consumes slot.
            return true;
        }
        return false;
    }

    bool empty() const { return head() == tail(); }

    size_t capacity() const { return capacity_ - 1; }
    size_t head() const { return head_.load(); }
    size_t tail() const { return tail_.load(); }

    size_t size() const
    {
        size_t head = head_;
        size_t tail = tail_;

        return tail >= head
                 ? tail - head
                 : tail + (capacity_ - head);
    }

private:
    std::unique_ptr<element_type[]> array_elem_;
    std::unique_ptr<std::atomic_bool[]> array_occupied_;
    size_t capacity_;
    std::atomic_size_t head_ = 0;
    std::atomic_size_t tail_ = 0;
};
} // namespace LOCK_FREE_CIRCULAR_DEPRECATED__
inline namespace LOCK__ {
template <typename Ty_>
class atomic_queue {
public:
    using element_type = Ty_;
    using difference_type = std::ptrdiff_t;
    using read_lock_type = std::shared_lock<std::shared_mutex>;
    using write_lock_type = std::lock_guard<std::shared_mutex>;

public:
    atomic_queue(size_t capacity)
        : queue_()
    {
    }

    template <typename RTy_>
    bool try_push(RTy_&& elem)
    {
        write_lock_type lock(queue_lock_);
        queue_.emplace_front(std::forward<RTy_>(elem));
        return true;
    }

    bool try_pop(Ty_& retval)
    {
        write_lock_type lock(queue_lock_);
        if (queue_.empty()) {
            return false;
        }

        retval = std::move(queue_.back());
        queue_.pop_back();
        return true;
    }

    bool empty() const
    {
        read_lock_type lock(queue_lock_);
        return queue_.empty();
    }

    size_t capacity() const { return -1; }
    size_t head() const { return 0; }
    size_t tail() const { return 0; }

    size_t size() const
    {
        read_lock_type lock(queue_lock_);
        return queue_.size();
    }

private:
    std::list<element_type> queue_;
    mutable std::shared_mutex queue_lock_;
};
} // namespace LOCK__
} // namespace kangsw
