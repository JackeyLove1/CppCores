#pragma once

#include <cstdint>
#include <pthread.h>
typedef pthread_t   ThreadHandleType;

/// thread scoped attribute and operations of current thread
class ThisThread {
    ThisThread();

    ~ThisThread();

public:
    static void Exit();

    static void Yield();

    static void Sleep(int64_t time_in_ms);

    static int GetLastErrorCode();

    static ThreadHandleType GetHandle();

    static int GetId();

    static bool IsMain();
};