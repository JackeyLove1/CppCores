#pragma once

#include <functional>
#include <type_traits>
#include <mutex>
#include <memory>
#include <atomic>

template<typename T>
class SingletonBase {
private:
    struct Holder {
        T value;

        std::atomic<bool> is_alive;

        template<typename ... Args>
        Holder(Holder **holder, Args &&... args): value{std::forward<Args>(args)...}, is_alive{true} {
            *holder = this;
        }

        ~Holder() {
            is_alive.store(false, std::memory_order_acquire);
        }
    };

protected:
    SingletonBase() {}

    virtual ~SingletonBase() {}


public:
    SingletonBase(const SingletonBase &) = delete;

    SingletonBase &operator=(const SingletonBase &) = delete;

    template<typename ...Args>
    static T *GetInstance(Args &&...args) {
        std::call_once(s_once_flag_, [&](Args &&... holder_args) {
            static Holder holder{&s_holder_, std::forward<Args>(holder_args)...};
        }, std::forward<Args>(args)...);
        return s_holder_->is_alive.load(std::memory_order_relaxed) ? &s_holder_->value : nullptr;
    }

    static bool IsAlive() {
        return s_holder_ != nullptr && s_holder_->is_alive.load(std::memory_order_relaxed);
    }

private:
    static Holder *s_holder_;
    static std::once_flag s_once_flag_;
};

template<typename T>
typename SingletonBase<T>::Holder *SingletonBase<T>::s_holder_;

template<typename T>
typename std::once_flag SingletonBase<T>::s_once_flag_;

template<typename T>
class Singleton : public SingletonBase<T> {

};