#pragma once
#include <mutex>

namespace kangsw::inline threads {

template <typename Ty_, typename Mtx_>
class _atomic_access_handle {
public:
    Ty_* operator->() { return _ref; }
    Ty_ const* operator->() const { return _ref; }
    Ty_& operator*() { return *_ref; }
    Ty_ const& operator*() const { return *_ref; }

    auto& value() { return *_ref; }
    auto const& value() const { return *_ref; }

    void release() { _ref = nullptr, _lck = {}; }

    operator bool() const { return !!_lck; }

private:
    template <typename, typename>
    friend class atomic_access;

    _atomic_access_handle(Ty_* ptr, std::unique_lock<Mtx_> lck)
        : _ref(ptr), _lck(std::move(lck)) {}

private:
    Ty_* _ref;
    std::unique_lock<Mtx_> _lck;
};

template <typename Ty_, typename Mtx_ = std::mutex>
class atomic_access : private Ty_ {
public:
    using Ty_::Ty_;
    using value_type = Ty_;
    using mutex_type = Mtx_;

    auto lock() {
        return _atomic_access_handle<Ty_, Mtx_>{this, std::unique_lock{_mt}};
    }

    auto lock(std::try_to_lock_t) {
        return _atomic_access_handle<Ty_, Mtx_>{this, std::unique_lock{_mt, std::try_to_lock}};
    }

private:
    Mtx_ _mt;
};

}  // namespace kangsw::inline threads