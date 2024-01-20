#pragma once

/**
 * @file macro.h
 * @brief 常用宏封装
 * @author JackyFan
*/

#include <cstdlib>
#include <cstring>
#include <type_traits>

#define LIKELY      __builtin_expect(!!(x), 1)
#define UNLIKELY    __builtin_expect(!!(x), 0)

#define CLASS_DELETE_FUNCTION(decl) decl = delete;

#define DISALLOW_COPY(TypeName)                         \
    CLASS_DELETE_FUNCTION(TypeName(const TypeName&))

#define DISALLOW_ASSIGN(TypeName)                               \
    CLASS_DELETE_FUNCTION(void operator=(const TypeName&))

#define DISALLOW_COPY_AND_ASSIGN(TypeName)                      \
    CLASS_DELETE_FUNCTION(TypeName(const TypeName&));            \
    CLASS_DELETE_FUNCTION(void operator=(const TypeName&))


#define CONCATENATE_IMPL(s1, s2) s1##s2
#define CONCATENATE(s1, s2) CONCATENATE_IMPL(s1, s2)
#define MAKE_UNIQUE_NAME(s) CONCATENATE(s, __COUNTER__)

template<typename To, typename From,
        typename std::enable_if_t<std::is_convertible_v<From, To>, void *> = nullptr>
inline To implicit_cast(From const &from) {
    return from;
}

template<typename Dest, typename Source, typename std::enable_if_t<
        sizeof(Dest) == sizeof(Source) && std::is_trivially_copyable_v<Source> &&
        std::is_trivially_copyable_v<Dest>, void *> = nullptr>
inline Dest bit_cast(Source const &source) {
    Dest dest{};
    std::memcpy(&dest, &source, sizeof(dest));
    return dest;
}