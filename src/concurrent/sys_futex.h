#pragma once

#include <unistd.h>
#include <ctime>
#include <syscall.h>
#include <linux/futex.h>

/*
 * 用于让当前线程等待 addr1 所指向的 futex 值变成 expected 指定的值。如果 addr1 上的值与
 * expected 相等，调用线程可能会挂起，直到超时或被其他线程唤醒
 * timeout 参数指定了线程挂起操作的超时时间。如果 timeout 是 NULL，则无限等待。
 */
inline int futex_wait_private(
        void *addr1, int expected, const timespec *timeout) {
    return syscall(SYS_futex, addr1, (FUTEX_WAIT | FUTEX_PRIVATE_FLAG),
                   expected, timeout, nullptr, 0);
}

/*
 * 用于唤醒至多 nwake 个在 addr1 指向的 futex 上等待的线程。
 * 如果 nwake 大于等待的线程数，则唤醒所有等待线程。该函数返回成功唤醒的线程数
 */
inline int futex_wake_private(void* addr1, int nwake) {
    return syscall(SYS_futex, addr1, (FUTEX_WAKE | FUTEX_PRIVATE_FLAG),
                   nwake, nullptr, nullptr, 0);
}

/*
 * 该函数可以将在 addr1 上等待的线程移动到另一个 futex addr2 上。执行时，它会唤醒至多 nwake 个线程，
 * 并将剩余的等待线程（如果有的话）重新排队到 addr2 指向的 futex 上。
 * 此操作通常用于管理大量线程时的优化，防止唤醒过多不必要的线程
 */
inline int futex_requeue_private(void* addr1, int nwake, void* addr2) {
    return syscall(SYS_futex, addr1, (FUTEX_REQUEUE | FUTEX_PRIVATE_FLAG),
                   nwake, NULL, addr2, 0);
}