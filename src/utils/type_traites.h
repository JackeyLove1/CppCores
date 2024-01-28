#pragma once

#include <type_traits>

/*
 * @brief try to implement some type traits
 */

template<typename T, T v>
struct integral_constant {
    static constexpr T value = v;
    using value_type = T;
    using type = integral_constant<T, v>;
};

template<typename T, T v> constexpr T integral_constant<T, v>::value;

typedef integral_constant<bool, true> true_type;
typedef integral_constant<bool, false> false_type;

// std::is_integral
template <typename T> struct is_integral : false_type {};
template<> struct is_integral<bool> : true_type {};
template<> struct is_integral<char> : true_type {};
template<> struct is_integral<unsigned char> : true_type {};
template<> struct is_integral<signed  char> : true_type {};
template<> struct is_integral<short> : true_type { };
template<> struct is_integral<unsigned short> : true_type { };
template<> struct is_integral<int> : true_type { };
template<> struct is_integral<unsigned int> : true_type { };
template<> struct is_integral<long> : true_type { };
template<> struct is_integral<unsigned long> : true_type { };
template<> struct is_integral<long long> : true_type { };
template<> struct is_integral<unsigned long long> : true_type { };

template<typename  T>
using is_integral_t = typename is_integral<T>::type ;

template<typename  T>
inline constexpr bool is_integral_v = is_integral<T>::value;

// std::is_floating_point
template <typename T> struct is_floating_point : false_type { };
template<> struct is_floating_point<float> : true_type { };
template<> struct is_floating_point<double> : true_type { };
template<> struct is_floating_point<long double> : true_type { };

template<typename  T>
using is_floating_point_t = typename is_integral<T>::type ;

template<typename  T>
inline constexpr bool is_floating_point_v = is_integral<T>::value;

template <typename T> struct is_pointer : false_type {};
template<typename T> struct is_pointer<T*> : true_type {};
