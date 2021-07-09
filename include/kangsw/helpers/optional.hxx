#pragma once
#include <array>
#include <stdexcept>

namespace kangsw ::inline helpers {

struct invalid_optional_access : std::logic_error {
    using logic_error::logic_error;
};

template <typename Ty_>
class optional {
public:
    using value_type = Ty_;
    enum { nbytes = sizeof Ty_ };

public:
    auto& operator*() const { return _get(); }
    auto& operator*() { return _get(); }
    auto operator->() const { return &_get(); }
    auto operator->() { return &_get(); }

    auto& value() const { return _get(); }
    auto& value() { return _get(); }

    template <typename... _Types>
    Ty_& emplace(_Types&&... args) {
        if (_init) { _destroy(); }
        _init = true;
        return *new (&_buf) Ty_(std::forward<_Types>(args)...);
    }

    template <typename RTy_>
    void reset(RTy_&& b) {
        if (_init) { _destroy(); }
        this->emplace(std::forward<RTy_>(b));

        _init = true;
    }

    void reset() {
        if (_init) {
            _destroy();
            _init = false;
        }
    }

    bool empty() const { return !_init; }
    operator bool() const { return !empty(); }

    ~optional() {
        reset();
    }

private:
    auto& _get() const {
        _check();
        return reinterpret_cast<const value_type&>(_buf);
    }

    auto& _get() {
        _check();
        return reinterpret_cast<value_type&>(_buf);
    }

    void _check() {
#if !defined(NDEBUG)
        if (!_init) { throw invalid_optional_access(""); }
#endif
    }

    void _destroy() { _get().~Ty_(); }

private:
    std::array<std::byte, nbytes> _buf;
    bool _init = false;
};
} // namespace kangsw::inline helpers