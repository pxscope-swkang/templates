#pragma once
#include <type_traits>

namespace kangsw::enum_arithmetic {

template <typename Enum_> Enum_ operator++(Enum_& l)
{
    return (Enum_)((std::underlying_type_t<Enum_>)l += 1);
}

template <typename Enum_> Enum_ operator++(Enum_& l, int)
{
    auto ret = l;
    (std::underlying_type_t<Enum_>)l += 1;
    return ret;
}

template <typename Enum_> Enum_ operator+(Enum_ l, Enum_ r)
{
    return (Enum_)((std::underlying_type_t<Enum_>)l + (std::underlying_type_t<Enum_>)r);
}

template <typename Enum_> Enum_ operator-(Enum_ l, Enum_ r)
{
    return (Enum_)((std::underlying_type_t<Enum_>)l - (std::underlying_type_t<Enum_>)r);
}

template <typename Enum_> Enum_ operator/(Enum_ l, Enum_ r)
{
    return (Enum_)((std::underlying_type_t<Enum_>)l / (std::underlying_type_t<Enum_>)r);
}

template <typename Enum_> Enum_ operator*(Enum_ l, Enum_ r)
{
    return (Enum_)((std::underlying_type_t<Enum_>)l * (std::underlying_type_t<Enum_>)r);
}

template <typename Enum_> Enum_ operator&(Enum_ l, Enum_ r)
{
    return (Enum_)((std::underlying_type_t<Enum_>)l & (std::underlying_type_t<Enum_>)r);
}

template <typename Enum_> Enum_ operator|(Enum_ l, Enum_ r)
{
    return (Enum_)((std::underlying_type_t<Enum_>)l | (std::underlying_type_t<Enum_>)r);
}

template <typename Enum_> Enum_ operator^(Enum_ l, Enum_ r)
{
    return (Enum_)((std::underlying_type_t<Enum_>)l ^ (std::underlying_type_t<Enum_>)r);
}

template <typename Enum_> Enum_ operator%(Enum_ l, Enum_ r)
{
    return (Enum_)((std::underlying_type_t<Enum_>)l ^ (std::underlying_type_t<Enum_>)r);
}

template <typename Enum_> Enum_ operator<<(Enum_ l, Enum_ r)
{
    return (Enum_)((std::underlying_type_t<Enum_>)l << (std::underlying_type_t<Enum_>)r);
}

template <typename Enum_> Enum_ operator>>(Enum_ l, Enum_ r)
{
    return (Enum_)((std::underlying_type_t<Enum_>)l >> (std::underlying_type_t<Enum_>)r);
}

template <typename Enum_> Enum_ operator+(std::underlying_type_t<Enum_> l, Enum_ r)
{
    return (Enum_)((std::underlying_type_t<Enum_>)l + (std::underlying_type_t<Enum_>)r);
}

template <typename Enum_> Enum_ operator-(std::underlying_type_t<Enum_> l, Enum_ r)
{
    return (Enum_)((std::underlying_type_t<Enum_>)l - (std::underlying_type_t<Enum_>)r);
}

template <typename Enum_> Enum_ operator/(std::underlying_type_t<Enum_> l, Enum_ r)
{
    return (Enum_)((std::underlying_type_t<Enum_>)l / (std::underlying_type_t<Enum_>)r);
}

template <typename Enum_> Enum_ operator*(std::underlying_type_t<Enum_> l, Enum_ r)
{
    return (Enum_)((std::underlying_type_t<Enum_>)l * (std::underlying_type_t<Enum_>)r);
}

template <typename Enum_> Enum_ operator&(std::underlying_type_t<Enum_> l, Enum_ r)
{
    return (Enum_)((std::underlying_type_t<Enum_>)l & (std::underlying_type_t<Enum_>)r);
}

template <typename Enum_> Enum_ operator|(std::underlying_type_t<Enum_> l, Enum_ r)
{
    return (Enum_)((std::underlying_type_t<Enum_>)l | (std::underlying_type_t<Enum_>)r);
}

template <typename Enum_> Enum_ operator^(std::underlying_type_t<Enum_> l, Enum_ r)
{
    return (Enum_)((std::underlying_type_t<Enum_>)l ^ (std::underlying_type_t<Enum_>)r);
}

template <typename Enum_> Enum_ operator%(std::underlying_type_t<Enum_> l, Enum_ r)
{
    return (Enum_)((std::underlying_type_t<Enum_>)l ^ (std::underlying_type_t<Enum_>)r);
}

template <typename Enum_> Enum_ operator<<(std::underlying_type_t<Enum_> l, Enum_ r)
{
    return (Enum_)((std::underlying_type_t<Enum_>)l << (std::underlying_type_t<Enum_>)r);
}

template <typename Enum_> Enum_ operator>>(std::underlying_type_t<Enum_> l, Enum_ r)
{
    return (Enum_)((std::underlying_type_t<Enum_>)l >> (std::underlying_type_t<Enum_>)r);
}

template <typename Enum_> Enum_ operator+(Enum_ l, std::underlying_type_t<Enum_> r)
{
    return (Enum_)((std::underlying_type_t<Enum_>)l + (std::underlying_type_t<Enum_>)r);
}

template <typename Enum_> Enum_ operator-(Enum_ l, std::underlying_type_t<Enum_> r)
{
    return (Enum_)((std::underlying_type_t<Enum_>)l - (std::underlying_type_t<Enum_>)r);
}

template <typename Enum_> Enum_ operator/(Enum_ l, std::underlying_type_t<Enum_> r)
{
    return (Enum_)((std::underlying_type_t<Enum_>)l / (std::underlying_type_t<Enum_>)r);
}

template <typename Enum_> Enum_ operator*(Enum_ l, std::underlying_type_t<Enum_> r)
{
    return (Enum_)((std::underlying_type_t<Enum_>)l * (std::underlying_type_t<Enum_>)r);
}

template <typename Enum_> Enum_ operator&(Enum_ l, std::underlying_type_t<Enum_> r)
{
    return (Enum_)((std::underlying_type_t<Enum_>)l & (std::underlying_type_t<Enum_>)r);
}

template <typename Enum_> Enum_ operator|(Enum_ l, std::underlying_type_t<Enum_> r)
{
    return (Enum_)((std::underlying_type_t<Enum_>)l | (std::underlying_type_t<Enum_>)r);
}

template <typename Enum_> Enum_ operator^(Enum_ l, std::underlying_type_t<Enum_> r)
{
    return (Enum_)((std::underlying_type_t<Enum_>)l ^ (std::underlying_type_t<Enum_>)r);
}

template <typename Enum_> Enum_ operator%(Enum_ l, std::underlying_type_t<Enum_> r)
{
    return (Enum_)((std::underlying_type_t<Enum_>)l ^ (std::underlying_type_t<Enum_>)r);
}

template <typename Enum_> Enum_ operator<<(Enum_ l, std::underlying_type_t<Enum_> r)
{
    return (Enum_)((std::underlying_type_t<Enum_>)l << (std::underlying_type_t<Enum_>)r);
}

template <typename Enum_> Enum_ operator>>(Enum_ l, std::underlying_type_t<Enum_> r)
{
    return (Enum_)((std::underlying_type_t<Enum_>)l >> (std::underlying_type_t<Enum_>)r);
}

} // namespace kangsw::enum_arithmetic