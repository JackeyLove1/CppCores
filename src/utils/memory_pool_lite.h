#pragma once

#include <algorithm>
#include <deque>
#include <numeric>
#include <functional>
#include <utility>
#include <memory>
#include <cstdlib>
#include "macro.h"

/**
 * @file memory_pool_lite.h
 * @brief 内存池实现
 * @reference: https://github.com/cacay/MemoryPool/tree/master
 */
class MemoryPool {
public:
    MemoryPool() = default;

    virtual ~MemoryPool() = default;

    virtual char *New(size_t size) = 0;

    virtual size_t PoolUsage() const = 0;

    virtual void Dispose(void *ptr) = 0;
};

class MemoryPoolSimpleImpl : public MemoryPool {
public:
    MemoryPoolSimpleImpl() = default;

    virtual ~MemoryPoolSimpleImpl() = default;

    char *New(size_t size) override { return reinterpret_cast<char *>(std::malloc(size)); };

    size_t PoolUsage() const override { return 0UL; };

    void Dispose(void *ptr) { std::free(ptr); };
};

class MemoryPoolLiteImpl : public MemoryPool {
public:
    MemoryPoolLiteImpl() : ptr_{0}, left_{0}, memory_{sizeof(*this)} {};

    virtual ~MemoryPoolLiteImpl() {
        std::for_each(blocks_.begin(), blocks_.end(), [](auto *p) { delete[] p; });
    };

    char *New(size_t size) override {
        if (left_ >= size) {
            left_ -= size;
            if (UNLIKELY(ptr_ == 0)) {
                return NewBlock(0);
            }
            char *ptr = reinterpret_cast<char *>(ptr_);
            ptr_ += size;
            return ptr;
        }
        return NewBlock(size);
    }

    size_t PoolUsage() const override {
        return memory_ + blocks_.size() * sizeof(blocks_.front());
    }

    void Dispose(void *ptr) {}

private:

    char *NewBlock(size_t block_size) {
        const size_t allocate_size = std::max(block_size, DefaultBlockSize);
        char *block = new char[allocate_size];
        blocks_.emplace_back(block);
        memory_ += allocate_size;
        if (allocate_size > block_size) {
            ptr_ = reinterpret_cast<uintptr_t>(block + block_size);
            left_ = allocate_size - block_size;
        }

        return block;
    }

    std::deque<char *> blocks_;
    uintptr_t ptr_{};
    size_t left_{};
    size_t memory_{};

    static constexpr size_t DefaultBlockSize = 64 * 1024;
};

template<typename T, size_t BlockSize = 4096>
class MemoryPoolLiteAllocator : public std::allocator<T> {
public:
    /* type traits */
    typedef T value_type;
    typedef T *pointer;
    typedef const T *const_pointer;
    typedef T &reference;
    typedef const T &const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef std::false_type propagate_on_container_copy_assignment;
    typedef std::true_type propagate_on_container_move_assignment;
    typedef std::true_type propagate_on_container_swap;

    template<typename U>
    struct rebind {
        typedef MemoryPoolLiteAllocator<T> other;
    };

    /* member functions */
    MemoryPoolLiteAllocator() noexcept;

    MemoryPoolLiteAllocator(const MemoryPoolLiteAllocator &) noexcept;

    MemoryPoolLiteAllocator(MemoryPoolLiteAllocator &&) noexcept;

    template<class U>
    MemoryPoolLiteAllocator(const MemoryPoolLiteAllocator<U> &) noexcept;

    ~MemoryPoolLiteAllocator() noexcept;

    MemoryPoolLiteAllocator &operator=(const MemoryPoolLiteAllocator &) = delete;

    MemoryPoolLiteAllocator &operator=(MemoryPoolLiteAllocator &&) noexcept;

    inline pointer address(reference value) const noexcept { return &value; }

    inline const_pointer address(const_reference value) const noexcept { return &value; }

    inline pointer allocate(size_type n = 1, const_pointer hint = 0);

    inline void deallocate(pointer p, size_type n = 1);

    inline size_type max_size() const noexcept { return static_cast<size_type>(-1) / sizeof(T); }

    template<class U, class ... Args>
    void construct(U *p, Args &&...args);

    template<class U>
    void destroy(U *p);

    template<class... Args>
    pointer newElement(Args &&... args);

    void deleteElement(pointer p);

private:
    union Slot_ {
        value_type element;
        Slot_ *next;
    };

    typedef char *data_pointer_;
    typedef Slot_ slot_type_;
    typedef Slot_ *slot_pointer_;

    slot_pointer_ currentBlock_;
    slot_pointer_ currentSlot_;
    slot_pointer_ lastSlot_;
    slot_pointer_ freeSlots_;

    size_type padPointer(data_pointer_ p, size_type align) const noexcept;

    void allocateBlock();

    static_assert(BlockSize >= 2 * sizeof(slot_type_), "BlockSize too small.");
};

template<typename T>
class SimpleAllocator : public std::allocator<T> {
public:
    template<typename U>
    struct rebind {
        typedef SimpleAllocator<U> other;
    };

public:
    T *allocate(size_t n, const void *hint = 0) {
        return reinterpret_cast<T *>(malloc(n * sizeof(T)));
    }

    void deallocate(T *p, size_t n) {
        std::free(reinterpret_cast<void *>(p));
    }
};