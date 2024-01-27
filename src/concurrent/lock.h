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

class SharedLock : public Lock {
public:
    SharedLock() = default;

    virtual ~SharedLock() override = default;

    SharedLock(const SharedLock &) = delete;

    SharedLock &operator=(const SharedLock &) = delete;

    virtual void lock_shared() noexcept = 0;

    virtual void unlock_shared() noexcept = 0;

    virtual bool try_lock_shared() noexcept = 0;
};