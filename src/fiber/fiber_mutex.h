#include <thread>
#include <functional>
#include <memory>
#include <cstdint>
#include <atomic>
#include <list>

#include <pthread.h>
#include <semaphore.h>

#include "fiber_nocopyable.h"

class Semaphore : public FiberNoncopyable {
private:
    sem_t m_semaphore;

public:
    explicit Semaphore(uint32_t count = 0);

    ~Semaphore();

    void wait();

    void notify();
};

class FiberSemaphore : public FiberNoncopyable {

};