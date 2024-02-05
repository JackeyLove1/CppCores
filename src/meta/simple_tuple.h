#include <type_traits>
#include <utility>
#include <functional>
#include <iostream>
#include <typeinfo>
#include <cxxabi.h>

/*
 * @brief: a simple implement of std::tuple
 * @author: JackyFan
 */

template<size_t Index, typename T>
class TupleLeaf {
public:
    T value_;

    static constexpr size_t index_{Index};

    explicit TupleLeaf(T &&value) : value_(std::forward<T>(value)) {};

    explicit TupleLeaf(T const &value) : value_(value) {};
};

template<size_t Index, typename  ... Ts>
class TupleImpl;

// Recursive template
template<size_t Index, typename T, typename ... Ts>
class TupleImpl<Index, T, Ts ...> : public TupleLeaf<Index, T>, public TupleImpl<Index + 1, Ts ...> {
public:
    explicit TupleImpl(T &&head, Ts &&... tails) : TupleLeaf<Index, T>(std::forward<T>(head)),
                                                   TupleImpl<Index + 1, Ts...>(std::forward<Ts>(tails)...) {}

    explicit TupleImpl(T const &head, Ts const &... tails) : TupleLeaf<Index, T>(head),
                                                             TupleImpl<Index + 1, Ts...>(tails...) {}

};

// Base case
template<size_t Index, typename T>
class TupleImpl<Index, T> : public TupleLeaf<Index, T> {
public:
    explicit TupleImpl(T &&value) : TupleLeaf<Index, T>(std::forward<T>(value)) {}

    explicit TupleImpl(T const &value) : TupleLeaf<Index, T>(value) {}

};

template<typename ... Ts>
class Tuple;

template<size_t Index, typename T>
T &tuple_get(TupleLeaf<Index, T> &leaf) {
    return leaf.value_;
}

template<size_t Index, typename T>
const T &tuple_get(TupleLeaf<Index, T> const &tuple) {
    return tuple.value_;
}

template<size_t Index, typename T>
T &&tuple_get(TupleLeaf<Index, T> &&leaf) {
    return std::forward<T>(leaf.value_);
}

template<typename  ... Ts>
class Tuple : public TupleImpl<0, Ts ...> {
public:
    explicit Tuple(Ts &&... values) : TupleImpl<0, Ts ...>(std::forward<Ts>(values)...) {}

    explicit Tuple(Ts const &... values) : TupleImpl<0, Ts ...>(values...) {}

    template<size_t Index>
    constexpr decltype(auto) get() noexcept {
        static_assert(Index < sizeof ... (Ts), "Tuple Index out of range!");
        return tuple_get<Index>(*this);
    }

    static constexpr size_t size() noexcept { return sizeof...(Ts); }
};

template<typename ... Ts>
decltype(auto) make_tuple(Ts &&... ts) {
    return Tuple<std::decay_t<Ts>...>(std::forward<Ts>(ts)...);
}

static inline void combine_hash(size_t &seed, size_t hash) {
    seed ^= hash + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}


static size_t hash_combine(size_t lhs, size_t rhs) {
    combine_hash(lhs, rhs);
    return lhs;
}

// Implement Tuple Hash Version 1
template<typename Tuple, size_t Index>
struct TupleHashHelper;

template<typename Tuple>
struct TupleHashHelper<Tuple, 0> {
    size_t operator()(Tuple const &tuple) const noexcept {
        return 0;
    }
};

template<typename Tuple, size_t Index>
struct TupleHashHelper {
    size_t operator()(Tuple const &tuple) const noexcept {
        size_t seed = TupleHashHelper<Tuple, Index - 1>{}(tuple);
        const auto &value = tuple_get<Index>(tuple);
        size_t hash = std::hash<typename std::decay_t<decltype(value)>>{}(value);
        return hash_combine(seed, hash);
    }
};

template<typename ...Ts>
struct HashTuple;

template<typename ... Ts>
struct HashTuple<Tuple<Ts...>> {
    size_t operator()(Tuple<Ts...> const &tuple) const noexcept {
        if constexpr (sizeof...(Ts) == 0) {
            return 0;
        }
        return TupleHashHelper<Tuple<Ts...>, (sizeof...(Ts)) - 1>{}(tuple);
    }
};

template<typename ... Ts>
size_t hash_tuple_struct(const Tuple<Ts...> &t) {
    return HashTuple<Tuple<Ts...>>{}(t);
}

template<typename Tuple, size_t...Is>
size_t hash_tuple_impl(Tuple const &t, std::index_sequence<Is...>) {
    std::size_t seed = 0;
    ((combine_hash(seed, std::hash<std::decay_t<decltype(tuple_get<Is>(t))>>{}(tuple_get<Is>(t)))), ...);
    return seed;
}

template<typename... Ts>
std::size_t hash_tuple_fold(const Tuple<Ts...> &t) {
    return hash_tuple_impl(t, std::index_sequence_for<Ts...>{});
}

// print helper
std::string Demangle(const char *mangled_name) {
    int status = 0;
    char *demangled_name = abi::__cxa_demangle(mangled_name, NULL, NULL, &status);
    std::string result(demangled_name);
    free(demangled_name);
    return result;
}

template<size_t IDX, size_t MAX, typename... Args>
struct PRINT_TUPLE {
    static void print(std::ostream &out, const Tuple<Args...> &t) {
        out << "Element " << IDX << " [" << Demangle(typeid(tuple_get<IDX>(t)).name()) << "]: " << tuple_get<IDX>(t)
            << std::endl;
        PRINT_TUPLE<IDX + 1, MAX, Args...>::print(out, t);
    }
};

template<size_t MAX, typename... Args>
struct PRINT_TUPLE<MAX, MAX, Args...> {
    static void print(std::ostream & /* out */, const Tuple<Args...> & /* t */) {}
};

template<typename... Args>
void print_tuple(std::ostream &out, const Tuple<Args...> &t) {
    PRINT_TUPLE<0, sizeof...(Args), Args...>::print(out, t);
}
