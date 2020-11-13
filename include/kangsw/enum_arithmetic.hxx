/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <ki6080@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.      Seungwoo Kang.
 * ----------------------------------------------------------------------------
 */
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