#include "lockfree_queue.h"

template<typename T, size_t N>
SpscLockFreeQueue<T, N>::SpscLockFreeQueue():r_(0), w_(0) {}

template<typename T, size_t N>
bool SpscLockFreeQueue<T, N>::push(const T &element) {
    /*
       The full check needs to be performed using the next write index not to
       miss the case when the read index wrapped and write index is at the end
     */
    const size_t w = w_.load(std::memory_order_relaxed);
    auto w_next = w + 1;
    if (w_next == N) {
        w_next = 0;
    }

    /*Fill Check*/
    const size_t r = r_.load(std::memory_order_acquire);
    if (w_next == r) {
        return false;
    }

    /*Write*/
    data_[w] = element;
    w_.store(w_next, std::memory_order_release);
    return true;
}

template<typename T, size_t N>
bool SpscLockFreeQueue<T, N>::pop(T &element) {
    /*Full Check*/
    size_t r = r_.load(std::memory_order_relaxed);
    const size_t w = w_.load(std::memory_order_acquire);
    if (r == w) {
        return false;
    }

    /*Remove Element*/
    element = data_[r];
    if (++r == N) {
        r = 0u;
    }

    /*Store the read index*/
    r_.store(r, std::memory_order_release);
    return true;
}

template<typename T, size_t N>
std::optional<T> SpscLockFreeQueue<T, N>::pop() {
    T element;
    if (pop(element)) {
        return element;
    }
    return {};
}


template<typename T, size_t N>
MpmcLockFreeQueue<T, N>::MpmcLockFreeQueue() : r_count_(0), w_count_(0) {

}

template<typename T, size_t N>
bool MpmcLockFreeQueue<T, N>::push(const T &element) {
    size_t w_count = w_count_.load(std::memory_order_relaxed);
    while (true) {
        const size_t index = w_count % N;
        const size_t push_count = data_[index].push_count.load(std::memory_order_acquire);
        const size_t pop_count = data_[index].push_count.load(std::memory_order_relaxed);

        if (push_count > pop_count) {
            return false;
        }

        const size_t revolution_count = w_count / N;
        const bool our_turn = revolution_count == push_count;

        if (our_turn) {
            /* Try to acquire the slot by bumping the monotonic write counter */
            if (w_count_.compare_exchange_weak(w_count, w_count + 1u, std::memory_order_relaxed)) {
                data_[index].val = element;
                data_[index].push_count.store(push_count + 1u, std::memory_order_release);
            }
        } else {
            w_count = w_count_.load(std::memory_order_relaxed);
        }
    }
}

template<typename T, size_t N>
bool MpmcLockFreeQueue<T, N>::pop(T &element) {
    size_t r_count = r_count_.load(std::memory_order_relaxed);
    while (true) {
        const size_t index = r_count % N;
        const size_t pop_count = data_[index].pop_count.load(std::memory_order_acquire);
        const size_t push_count = data_[index].push_count.load(std::memory_order_relaxed);

        if (pop_count == push_count) {
            return false;
        }

        const size_t revolution_count = r_count / N;
        const bool our_turn = revolution_count == pop_count;

        if (our_turn) {
            /* Try to acquire the slot by bumping the monotonic write counter */
            if (r_count_.compare_exchange_weak(r_count, r_count + 1u, std::memory_order_relaxed)) {
                element = data_[index].val;
                data_[index].push_count.store(pop_count + 1u, std::memory_order_release);
                return true;
            }
        } else {
            r_count = r_count_.load(std::memory_order_relaxed);
        }
    }
}

template<typename T, size_t N>
std::optional<T> MpmcLockFreeQueue<T, N>::pop() {
    T element;
    bool result = pop(element);

    if (result) {
        return element;
    }
    return {};
}

