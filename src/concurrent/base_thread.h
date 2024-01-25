#pragma once

#include <string>
#include <pthread.h>

typedef pthread_t ThreadHandleType;

class BaseThread;

class ThreadAttributes {
    friend class BaseThread;

public:
    explicit ThreadAttributes();

    ~ThreadAttributes();

    ThreadAttributes &SetName(std::string name);

    ThreadAttributes &SetStackSize(size_t size);

    ThreadAttributes &SetDetached(bool detached);

    ThreadAttributes &SetPriority(int priority);

    bool IsDetached() const;

private:
    std::string m_name;
    pthread_attr_t m_attr;
};

class BaseThread {
    friend class Thread;

public:
    typedef ThreadHandleType HandleType;

    BaseThread();

    explicit BaseThread(const ThreadAttributes &attributes);

public:
    virtual ~BaseThread() {};

    // Start or TryStary Must be called to start a thread, or the thread will
    // not be created.
    // But can't be called again unless joined or detached.

    // Return false if failed
    bool TryStart();

    // If failed, abort the process.
    void Start();

    // Wait for thread termination
    // the thread must be started and not detached
    bool Join();

    // Whether the thread is still alive
    bool IsAlive() const;

    // Is fitable to call Join?
    bool IsJoinable() const;

    // Get handle
    HandleType GetHandle() const;

    // Get numeric thread id
    int GetId() const;

protected:
    // This virtual function will be called before the thread exiting
    virtual void OnExit();

private:
    int DoStart();

    // BaseThread should not support Detach, because the running thread can't
    // control the memory of BaseThread object. So if it is detached and the
    // BaseThread destructed, the running thread will access the destructed
    // object, make it private.
    bool DoDetach();

    // The derived class must override this function as the thread entry point
    virtual void Entry() = 0;

    static void Cleanup(void *param);

private:
    static void *StaticEntry(void *inThread);

private:
    ThreadAttributes m_attributes;
    HandleType m_handle;
    int m_id;
    volatile bool m_is_alive;
};