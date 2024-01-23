#include <cstdint>
#include "memory_pool_lite.h"

template<typename T, size_t BlockSize>
inline typename MemoryPoolLiteAllocator<T, BlockSize>::size_type
MemoryPoolLiteAllocator<T, BlockSize>::padPointer(data_pointer_ p, size_type align)
const noexcept {
    uintptr_t result = reinterpret_cast<uintptr_t>(p);
    return ((align - result) % align);
}

template<typename T, size_t BlockSize>
MemoryPoolLiteAllocator<T, BlockSize>::MemoryPoolLiteAllocator() noexcept:
        currentBlock_{nullptr},
        currentSlot_{nullptr},
        lastSlot_{nullptr},
        freeSlots_{nullptr} {}

template<typename T, size_t BlockSize>
MemoryPoolLiteAllocator<T, BlockSize>::MemoryPoolLiteAllocator(const MemoryPoolLiteAllocator &) noexcept
        :MemoryPoolLiteAllocator() {}

template<typename T, size_t BlockSize>
MemoryPoolLiteAllocator<T, BlockSize>::MemoryPoolLiteAllocator(MemoryPoolLiteAllocator &&other) noexcept
        :MemoryPoolLiteAllocator() {
    currentBlock_ = other.currentBlock_;
    other.currentBlock_ = nullptr;
    currentSlot_ = other.currentSlot_;
    lastSlot_ = other.lastSlot_;
    freeSlots_ = other.freeSlots_;
}

template<typename T, size_t BlockSize>
template<class U>
MemoryPoolLiteAllocator<T, BlockSize>::MemoryPoolLiteAllocator(const MemoryPoolLiteAllocator<U> &memoryPool) noexcept
        : MemoryPoolLiteAllocator() {}

template<typename T, size_t BlockSize>
MemoryPoolLiteAllocator<T, BlockSize> &
MemoryPoolLiteAllocator<T, BlockSize>::operator=(MemoryPoolLiteAllocator<T, BlockSize> &&other) noexcept {
    if (this != &other) {
        std::swap(currentBlock_, other.currentBlock_);
        currentSlot_ = other.currentSlot_;
        lastSlot_ = other.lastSlot_;
        freeSlots_ = other.freeSlots_;
    }
    return *this;
}

template<typename T, size_t BlockSize>
MemoryPoolLiteAllocator<T, BlockSize>::~MemoryPoolLiteAllocator() noexcept {
    slot_pointer_ curr = currentBlock_;
    while (curr != nullptr) {
        slot_pointer_ next = curr->next;
        operator delete(reinterpret_cast<void *>(curr));
        curr = next;
    }
}

template<typename T, size_t BlockSize>
void
MemoryPoolLiteAllocator<T, BlockSize>::allocateBlock() {
    // Allocate space for the new block and store a pointer to the previous one
    data_pointer_ newBlock = reinterpret_cast<data_pointer_>
    (operator new(BlockSize));
    reinterpret_cast<slot_pointer_>(newBlock)->next = currentBlock_;
    currentBlock_ = reinterpret_cast<slot_pointer_>(newBlock);
    // Pad block body to staisfy the alignment requirements for elements
    data_pointer_ body = newBlock + sizeof(slot_pointer_);
    size_type bodyPadding = padPointer(body, alignof(slot_type_));
    currentSlot_ = reinterpret_cast<slot_pointer_>(body + bodyPadding);
    lastSlot_ = reinterpret_cast<slot_pointer_>
    (newBlock + BlockSize - sizeof(slot_type_) + 1);
}

template<typename T, size_t BlockSize>
inline typename MemoryPoolLiteAllocator<T, BlockSize>::pointer
MemoryPoolLiteAllocator<T, BlockSize>::allocate(size_type n, const_pointer hint) {
    if (freeSlots_ != nullptr) {
        pointer result = reinterpret_cast<pointer>(freeSlots_);
        freeSlots_ = freeSlots_->next;
        return result;
    } else {
        if (currentSlot_ >= lastSlot_)
            allocateBlock();
        return reinterpret_cast<pointer>(currentSlot_++);
    }
}

template<typename T, size_t BlockSize>
inline void
MemoryPoolLiteAllocator<T, BlockSize>::deallocate(pointer p, size_type n) {
    if (p != nullptr) {
        reinterpret_cast<slot_pointer_>(p)->next = freeSlots_;
        freeSlots_ = reinterpret_cast<slot_pointer_>(p);
    }
}

template<typename T, size_t BlockSize>
template<class U, class... Args>
inline void
MemoryPoolLiteAllocator<T, BlockSize>::construct(U *p, Args &&... args) {
    new(p) U(std::forward<Args>(args)...);
}


template<typename T, size_t BlockSize>
template<class U>
inline void
MemoryPoolLiteAllocator<T, BlockSize>::destroy(U *p) {
    p->~U();
}


template<typename T, size_t BlockSize>
template<class... Args>
inline typename MemoryPoolLiteAllocator<T, BlockSize>::pointer
MemoryPoolLiteAllocator<T, BlockSize>::newElement(Args &&... args) {
    pointer result = allocate();
    construct<value_type>(result, std::forward<Args>(args)...);
    return result;
}


template<typename T, size_t BlockSize>
inline void
MemoryPoolLiteAllocator<T, BlockSize>::deleteElement(pointer p) {
    if (p != nullptr) {
        p->~value_type();
        deallocate(p);
    }
}

