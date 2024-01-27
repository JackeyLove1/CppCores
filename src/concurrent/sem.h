#pragma  once

#include <cerrno>
#include <cstdlib>
#include <string>
#include <sys/types.h>
#include <semaphore.h>

class SemaphoreOps {
public:
    static void Acquire(sem_t *sem) {
        sem_wait(sem);
    }

    static bool TryAcquire(sem_t *sem) {
        return sem_trywait(sem) == 0;
    }

    static bool TimedAcquire(sem_t *sem, int64_t timeout) {
        struct timespec ts{};
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_nsec += timeout;
        return sem_timedwait(sem, &ts) == 0;
    }

    static void Release(sem_t *sem) {
        sem_post(sem);
    }

    static unsigned int GetValue(const sem_t *sem) {
        int value;
        sem_getvalue(const_cast<sem_t *>(sem), &value);
        return value;
    }
};

class Semaphore {
    Semaphore(const Semaphore &) = delete;

    Semaphore &operator=(const Semaphore &) = delete;

public:
    explicit Semaphore(unsigned int value) {
        sem_init(&m_sem, false, value);
    }

    ~Semaphore() {
        sem_destroy(&m_sem);
    }

    void Acquire() {
        SemaphoreOps::Acquire(&m_sem);
    }

    bool TryAcquire() {
        return SemaphoreOps::TryAcquire(&m_sem);
    }

    bool TimedAcquire(int64_t timeout) // in ms
    {
        return SemaphoreOps::TimedAcquire(&m_sem, timeout);
    }

    void Release() {
        return SemaphoreOps::Release(&m_sem);
    }

    // Usually get value is only used for debug propose,
    // be careful your design if you need it.
    unsigned int GetValue() const {
        return SemaphoreOps::GetValue(&m_sem);
    }

private:
    sem_t m_sem;
};