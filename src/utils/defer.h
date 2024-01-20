#include <functional>
#include <type_traits>
#include "macro.h"

class Defer {
public:
    template<typename Func>
    explicit Defer(Func &&f): f_(std::forward<Func>(f)) {}

    ~Defer() { std::invoke(f_); }

    Defer(const Defer &) = delete;

    Defer &operator=(const Defer &) = delete;

private:
    std::function<void()> f_;
};

#define DEFER(code) \
    Defer MAKE_UNIQUE_NAME(_defer_)(code);
