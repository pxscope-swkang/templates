/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <ki6080@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.      Seungwoo Kang.
 * ----------------------------------------------------------------------------
 */
#pragma once
#include <atomic>
#include <list>
#include <memory>
#include <mutex>
#include <shared_mutex>

namespace kangsw {
inline namespace threads {
template <typename Ty_>
class atomic_queue {
public:
    using element_type = Ty_;
    using difference_type = std::ptrdiff_t;
    using read_lock_type = std::shared_lock<std::shared_mutex>;
    using write_lock_type = std::lock_guard<std::shared_mutex>;

public:
    atomic_queue(size_t capacity) :
        queue_() {
    }

    template <typename RTy_>
    bool try_push(RTy_&& elem) {
        write_lock_type lock(queue_lock_);
        queue_.emplace_front(std::forward<RTy_>(elem));
        return true;
    }

    bool try_pop(Ty_& retval) {
        write_lock_type lock(queue_lock_);
        if (queue_.empty()) {
            return false;
        }

        retval = std::move(queue_.back());
        queue_.pop_back();
        return true;
    }

    bool empty() const {
        read_lock_type lock(queue_lock_);
        return queue_.empty();
    }

    size_t capacity() const { return -1; }
    size_t head() const { return 0; }
    size_t tail() const { return 0; }

    size_t size() const {
        read_lock_type lock(queue_lock_);
        return queue_.size();
    }

private:
    std::list<element_type> queue_;
    mutable std::shared_mutex queue_lock_;
};
} // namespace threads
} // namespace kangsw
