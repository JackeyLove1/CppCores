#include <type_traits>
#include <utility>
#include <functional>
#include <iostream>

/*
 * @brief A simple implement if std::integer_sequence
 * @author: JackyFan
 */

template<typename T, T ... Ints>
struct integer_sequence {
    static_assert(std::is_integral_v<T>, "integer_sequence can only be instantiated with an integral type");
    using value_type = T;

    static constexpr size_t size() noexcept { return sizeof ... (Ints); }
};


template<typename T, size_t N, T ... Ints>
struct make_integer_sequence_impl;

template<typename T, T ... Ints>
struct make_integer_sequence_impl<T, 0, Ints ...> {
    static_assert(std::is_integral_v<T>, "make_integer_sequence can only be instantiated with an integral type");
    using type = integer_sequence<T, Ints ...>;
};

template<typename T, size_t N, T ... Ints>
struct make_integer_sequence_impl : make_integer_sequence_impl<T, N - 1, N - 1, Ints...> {

};

template<typename T, std::size_t N>
using make_integer_sequence = typename make_integer_sequence_impl<T, N>::type;

template<size_t N>
using make_index_sequence = make_integer_sequence<size_t, N>;


template<typename T, T... ints>
void print_sequence(integer_sequence<T, ints...> int_seq) {
    std::cout << "The sequence of size " << int_seq.size() << ": ";
    ((std::cout << ints << ' '), ...);
    std::cout << '\n';
}