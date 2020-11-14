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
#include <shared_mutex>

namespace kangsw {
/**
 * 프로세스가 스코프 바깥으로 나가는 것을 방지.
 * 멀티스레드 환경에서, 클래스 멤버 가장 아래쪽에 배치하여 소멸 시점을 제어할 수 있습니다.
 * 중첩해서 잠글 수 있는 일종의 경량 뮤텍스이며, 반드시 사용 후 해제되어야 합니다.
 */
class destruction_guard {
public:
    void lock() { lock_.fetch_add(1); }
    bool try_lock() { return lock_.fetch_add(1), true; }
    void unlock() { lock_.fetch_sub(1); }
    bool is_locked() const { return lock_.load(); }

    ~destruction_guard() {
        for (; lock_.load(); std::this_thread::sleep_for(poll_interval)) {}
    }

public:
    std::chrono::system_clock::duration poll_interval = std::chrono::milliseconds(1);

private:
    mutable std::atomic_int64_t lock_;
};

} // namespace kangsw
