#include <cerrno>
#include <cstring>
#include <thread>
#include <chrono>

#include <pthread.h>
#include <syscall.h>
#include <unistd.h>

#include "this_thread.h"

ThreadHandleType ThisThread::GetHandle() {
    return ::pthread_self();
}

int ThisThread::GetId() {
    static thread_local pid_t tid = 0;
    if (tid == 0)
        tid = syscall(SYS_gettid);
    return tid;
}

void ThisThread::Exit() {
    pthread_exit(nullptr);
}

void ThisThread::Sleep(int64_t time_in_ms) {
    if (time_in_ms >= 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(time_in_ms));
    } else {
        pause();
    }
}

void ThisThread::Yield() {
    sched_yield();
}

int ThisThread::GetLastErrorCode() {
    return errno;
}

bool ThisThread::IsMain() {
    return ThisThread::GetId() == getpid();
}