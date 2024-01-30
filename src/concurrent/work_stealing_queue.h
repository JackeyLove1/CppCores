#include <atomic>
#include <memory>
#include <functional>
#include <type_traits>

#include <glog/logging.h>

#include "macro.h"

/*
 * @brief lockfree queue for work stealing
 */

template<typename T, typename  =
std::enable_if<std::is_default_constructible_v<T>
               && std::is_copy_assignable_v<T>, bool>>
class WorkStealingQueue {
private:
    std::atomic<size_t> _bottom;
    size_t _capacity;
    std::unique_ptr<T[]> _buffer;
    CACHELINE_ALIGNOF std::atomic<size_t> _top;

public:
    WorkStealingQueue() :
            _bottom(1),
            _capacity(0),
            _buffer(nullptr),
            _top(1) {
    }

    ~WorkStealingQueue() {};

    WorkStealingQueue(const WorkStealingQueue &) = delete;

    WorkStealingQueue &operator=(const WorkStealingQueue &) = delete;

    int init(size_t capacity) {
        if (_capacity != 0) {
            LOG(ERROR) << "WorkStealingQueue Already init, capacity: " << _capacity;
            return -1;
        }
        if (capacity == 0) {
            LOG(ERROR) << "Invalid capacity: " << capacity;
            return -1;
        }
        if (capacity & (capacity - 1)) {
            LOG(ERROR) << "Invalid capacity=" << capacity
                       << " which must be power of 2";
            return -1;
        }

        _buffer.reset(new(std::nothrow) T[capacity]);
        if (_buffer == nullptr) {
            LOG(ERROR) << "Failed to allocate memory for WorkStealingQueue, capacity: " << capacity;
            return -1;
        }
        _capacity = capacity;
        return 0;
    }

    // Push an item into the queue.
    // Returns true on pushed.
    // May run in parallel with steal().
    // Never run in parallel with pop() or another push().
    bool push(const T &value) {
        const auto b = _bottom.load(std::memory_order_relaxed);
        const auto t = _top.load(std::memory_order_acquire);
        if (b >= t + _capacity) { // Full queue
            return false;
        }
        // The expression b & (_capacity - 1) is effectively used to ensure
        // that the index b is wrapped around to always fall within the boundaries
        // of _buffer. This makes the buffer circular, meaning that if b exceeds _capacity,
        // it will be wrapped around to the start instead of going out of bounds.
        // For example, if _capacity is 8, _capacity - 1 would be 7 which is 111 in binary.
        // Performing a bitwise AND with any index b will essentially reduce b to its last three bits,
        // ensuring the result is between 0 and 7 (inclusive). If b is 8, which is 1000 in binary,
        // then 8 & 7 yields 0, thus it wraps around.
        // In a circular buffer, this technique is a very efficient way to cycle through buffer
        // indices without needing conditional checks for boundaries, which makes the code compact and fast.
        _buffer[b & (_capacity - 1)] = value;
        _bottom.store(b + 1, std::memory_order_release);
        return true;
    }

    // Pop an item from the queue.
    // Returns true on popped and the item is written to `val'.
    // May run in parallel with steal().
    // Never run in parallel with push() or another pop().
    bool pop(T *val) {
        const auto b = _bottom.load(std::memory_order_relaxed);
        auto t = _top.load(std::memory_order_acquire);
        if (t >= b) {
            // fast check since we call pop() in each sched.
            // Stale _top which is smaller should not enter this branch.
            return false;
        }
        const auto newb = b - 1;
        _bottom.store(newb, std::memory_order_release);
        std::atomic_thread_fence(std::memory_order_acq_rel);
        t = _top.load(std::memory_order_relaxed);
        if (t > newb) {
            _bottom.store(b, std::memory_order_relaxed);
            return false;
        }
        *val = _buffer[newb & (_capacity - 1)];
        if (t != newb) {
            return true;
        }
        // Single last element, compete with steal()
        const bool popped = _top.compare_exchange_strong(
                t, t + 1, std::memory_order_seq_cst, std::memory_order_relaxed);
        _bottom.store(b, std::memory_order_relaxed);
        return popped;
    }

    // Steal one item from the queue.
    // Returns true on stolen.
    // May run in parallel with push() pop() or another steal().
    bool steal(T *val) {
        auto t = _top.load(std::memory_order_relaxed);
        auto b = _bottom.load(std::memory_order_acquire);
        if (t >= b) {
            // Permit false negative for performance considerations.
            return false;
        }
        do {
            std::atomic_thread_fence(std::memory_order_seq_cst);
            b = _bottom.load(std::memory_order_acquire);
            if (t >= b) {
                return false;
            }
            *val = _buffer[t & (_capacity - 1)];
        } while (!_top.compare_exchange_strong(t, t + 1, std::memory_order_seq_cst, std::memory_order_relaxed));
        return true;
    }

    size_t volatile_size() const {
        const auto b = _bottom.load(std::memory_order_relaxed);
        const auto t = _top.load(std::memory_order_relaxed);
        return (b <= t ? 0 : (b - t));
    }
};