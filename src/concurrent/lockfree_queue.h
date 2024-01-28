#pragma once

#include <atomic>
#include <cstddef>
#include <type_traits>
#include <optional>

#include "lockfree.h"

/**
* @require: __cplusplus >= 201703L
*/

/**
 * @brief Abstract class for queue
 */

template<typename T>
class Queue {
public:
    Queue() = default;

    virtual ~Queue() = default;

    Queue(const Queue &) = delete;

    Queue &operator=(const Queue &) = delete;

    virtual bool push(const T &) = 0;

    virtual bool pop(T &) = 0;

    virtual std::optional<T> pop() = 0;
};

/**
* @brief Single producer single consumer lock free queue
*/
template<typename T, size_t N>
class SpscLockFreeQueue : public Queue<T> {
    static_assert(std::is_trivial<T>::value, "T must be trivial type");
    static_assert(N > 2, "Queue size must be greater than 2");
public:

    SpscLockFreeQueue();

    ~SpscLockFreeQueue() override = default;

    /**
     * @brief Adds an element into the queue.
     * Should only be called from the producer thread.
     * @param[in] element
     * @retval Operation success
     */
    bool push(const T &element) override;

    /**
     * @brief Removes an element from the queue.
     * Should only be called from the consumer thread.
     * @param[out] element
     * @retval Operation success
     */
    bool pop(T &element) override;

    /**
     * @brief Removes an element from the queue.
     * Should only be called from the consumer thread.
     * @retval Either the element or nothing
     */
    std::optional<T> pop() override;

private:
    T data_[N];

private:
    alignas(LOCKFREE_CACHELINE_LENGTH) std::atomic_size_t r_;
    alignas(LOCKFREE_CACHELINE_LENGTH) std::atomic_size_t w_;
};

/**
 * @brief Multi producer Multi consumer lock free queue
 * @attention the code has some problems!
*/
template<typename T, size_t N>
class MpmcLockFreeQueue : public Queue<T> {
    static_assert(std::is_trivial<T>::value, "T must be trivial type");
    static_assert(N > 2, "Queue size must be greater than 2");
public:
    MpmcLockFreeQueue();

    ~MpmcLockFreeQueue() override = default;

    bool push(const T &element) override;

    bool pop(T &element) override;

    std::optional<T> pop() override;

private:
    struct Slot {
        T val;
        std::atomic_size_t pop_count;
        std::atomic_size_t push_count;

        Slot() : pop_count(0U), push_count(0U) {}
    };

    Slot data_[N];

private:
    alignas(LOCKFREE_CACHELINE_LENGTH) std::atomic_size_t r_count_;
    alignas(LOCKFREE_CACHELINE_LENGTH) std::atomic_size_t w_count_;
};

#include "lockfree_queue_impl.h"