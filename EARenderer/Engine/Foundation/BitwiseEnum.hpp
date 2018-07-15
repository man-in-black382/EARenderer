//
//  BitwiseEnum.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 27.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef BitwiseEnum_hpp
#define BitwiseEnum_hpp

#include <type_traits>

// http://blog.bitwigglers.org/using-enum-classes-as-type-safe-bitmasks/

template<typename Enum>
struct EnableBitMaskOperators
{
    static const bool enable = false;
};

template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
operator |(Enum lhs, Enum rhs)
{
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<Enum> (
                              static_cast<underlying>(lhs) |
                              static_cast<underlying>(rhs)
                              );
}

template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
operator &(Enum lhs, Enum rhs)
{
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<Enum> (
                              static_cast<underlying>(lhs) &
                              static_cast<underlying>(rhs)
                              );
}

template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
operator ^(Enum lhs, Enum rhs)
{
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<Enum> (
                              static_cast<underlying>(lhs) ^
                              static_cast<underlying>(rhs)
                              );
}

template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
operator ~(Enum rhs)
{
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<Enum> (
                              ~static_cast<underlying>(rhs)
                              );
}

template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum&>::type
operator |=(Enum &lhs, Enum rhs)
{
    using underlying = typename std::underlying_type<Enum>::type;
    lhs = static_cast<Enum> (
                             static_cast<underlying>(lhs) |
                             static_cast<underlying>(rhs)
                             );
    return lhs;
}

template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum&>::type
operator &=(Enum &lhs, Enum rhs)
{
    using underlying = typename std::underlying_type<Enum>::type;
    lhs = static_cast<Enum> (
                             static_cast<underlying>(lhs) &
                             static_cast<underlying>(rhs)
                             );
    return lhs;
}

template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum&>::type
operator ^=(Enum &lhs, Enum rhs)
{
    using underlying = typename std::underlying_type<Enum>::type;
    lhs = static_cast<Enum> (
                             static_cast<underlying>(lhs) ^
                             static_cast<underlying>(rhs)
                             );
    return lhs;
}

#define ENABLE_BITMASK_OPERATORS(x)  \
template<>                           \
struct EnableBitMaskOperators<x>     \
{                                    \
static const bool enable = true; \
};

#endif /* BitwiseEnum_hpp */
