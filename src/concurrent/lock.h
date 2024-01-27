#pragma once

class Lock {
public:
    Lock() = default;

    virtual ~Lock() = default;

    Lock(const Lock &) = delete;

    Lock &operator=(const Lock &) = delete;

    virtual void lock() noexcept = 0;

    virtual void unlock() noexcept = 0;

    virtual bool try_lock() noexcept = 0;

};