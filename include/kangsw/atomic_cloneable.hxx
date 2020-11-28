#pragma once
#include <atomic>

namespace kangsw::inline threads {

template <typename Ty_,
          std::memory_order ImplicitStorePolicy_ = std::memory_order_seq_cst,
          std::memory_order ImplicitLoadPolicy_ = std::memory_order_relaxed>
class atomic_cloneable : public std::atomic<Ty_> {
    using super = std::atomic<Ty_>;

public:
    static constexpr std::memory_order implicit_load_policy = ImplicitLoadPolicy_;
    static constexpr std::memory_order implicit_store_policy = ImplicitLoadPolicy_;

public:
    atomic_cloneable() = default;

    template <typename RTy_>
    atomic_cloneable(std::atomic<RTy_> const& r) { super::store(r.load(implicit_load_policy), implicit_store_policy); }
    template <typename RTy_>
    atomic_cloneable& operator=(std::atomic<RTy_> const& r) { return super::store(r.load(implicit_load_policy), implicit_store_policy), *this; }

    template <typename DTy_>
    operator std::atomic<DTy_>() const { return super::load(implicit_load_policy); }
    operator Ty_() const { return super::load(implicit_load_policy); }

    atomic_cloneable(atomic_cloneable&& r) = delete;
};

} // namespace kangsw::inline threads