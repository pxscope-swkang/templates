#pragma once
#include <utility>

namespace kangsw:: inline misc {

template <typename Ty_>
class ptr_proxy {
public:
    using element_type = Ty_;
    using pointer = Ty_*;
    using const_pointer = Ty_ const*;
    using reference = Ty_&;
    using const_reference = Ty_ const&;

public:
    operator const_reference() const { return get(); }
    operator reference() { return get(); }
    ptr_proxy(pointer ptr = nullptr) :
        ptr_(ptr) {}

    ptr_proxy(const ptr_proxy&) = default;
    ptr_proxy(ptr_proxy&&) = default;
    ptr_proxy& operator=(const ptr_proxy&) = default;
    ptr_proxy& operator=(ptr_proxy&&) = default;

    template <typename RTy_>
    reference operator=(RTy_&& r) { return *ptr_ = std::forward<RTy_>(r); }

    reference get() { return *ptr_; }
    const_reference get() const { return *ptr_; }

private:
    Ty_* ptr_;
};

} // namespace kangsw
