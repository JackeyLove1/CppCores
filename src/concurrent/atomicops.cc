#include "atomicops.h"

// 32-bit low-level operations on any platform.

inline Atomic32 NoBarrier_CompareAndSwap(volatile Atomic32 *ptr,
                                         Atomic32 old_value,
                                         Atomic32 new_value) {
    Atomic32 prev;
    __asm__ __volatile__("lock; cmpxchgl %1,%2"
            : "=a" (prev)
            : "q" (new_value), "m" (*ptr), "0" (old_value)
            : "memory");
    return prev;
}

