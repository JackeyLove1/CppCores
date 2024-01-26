#include <type_traits>
#include <tuple>
#include <functional>
#include <memory>
#include <utility>

template<typename R>
class ClosureBase {
public:
    ClosureBase() = default;

    virtual ~ClosureBase() = default;

    virtual R Run() = 0;

};

template<typename R, typename ... Args>
class Closure : public ClosureBase<R> {
public:
    using FunctionType = std::function<R(Args...)>;
    using ReturnType = R;
    using ArgPack = std::tuple<Args...>;

    explicit Closure(FunctionType func, Args ... args) : m_func{func},
                                                         m_args{args...} {}

    ~Closure() override = default;

    ReturnType Run() override {
        return std::apply(m_func, m_args);
    }

private:
    FunctionType m_func;
    ArgPack m_args;
};

/*
template<typename R, typename... Args>
auto make_closure(std::function<R(Args...)> func, Args &&... args) {
    return [&]() {
        return func(std::forward<Args>(args)...);
    };
}
*/


template<typename R, typename... Args>
auto make_closure(std::function<R(Args...)> func, Args ... args) {
    return Closure<R, Args...>(func, args...);
}

template<typename Func, typename... Args>
decltype(auto) make_closure(Func func, Args &&... args) {
    return [&]() {
        return func(std::forward<Args>(args)...);
    };
}