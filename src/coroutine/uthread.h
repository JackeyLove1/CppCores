#pragma once

#include <atomic>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <memory>
#include <thread>
#include <ucontext.h>
#include <utility>

static constexpr size_t DEFAULT_STACK_SZIE = 1024 * 128;
static constexpr size_t MAX_UTHREAD_SIZE = 1024;
using Func = std::function<void(void *)>;

enum class ThreadState : uint8_t {
    FREE = 0,
    RUNNABLE = 1,
    RUNNING = 2,
    SUSPEND = 3,
};

struct schedule_t;

struct uthread_t {
    ucontext_t ctx;
    Func func;
    void *arg;
    ThreadState state;
    char stack[DEFAULT_STACK_SZIE];

    uthread_t()
            : ctx{}, func(nullptr), arg(nullptr), state(ThreadState::FREE), stack{} {}
};

struct schedule_t {
    ucontext_t main;
    int running_thread;
    int max_index;
    uthread_t *threads;

    schedule_t()
            : running_thread{-1},
              max_index{0},
              threads{new uthread_t[MAX_UTHREAD_SIZE]} {}

    ~schedule_t() { delete[] threads; }
};

static void uthread_body(schedule_t *ps);

/**
 * @brief create uthreads
 * @param schedule
 * @return
 */
int uthread_create(schedule_t &schedule, Func func, void *args);

void uthread_yield(schedule_t &schedule);

void uthread_resume(schedule_t &schedule, int id);

int schedule_finished(const schedule_t &schedule);

void uthread_resume(schedule_t &schedule, int id) {
    if (id < 0 || id >= schedule.max_index) {
        return;
    }

    uthread_t *t = &(schedule.threads[id]);

    if (t->state == ThreadState::SUSPEND) {
        swapcontext(&(schedule.main), &(t->ctx));
    }
}

void uthread_yield(schedule_t &schedule) {
    if (schedule.running_thread != -1) {
        uthread_t *t = &(schedule.threads[schedule.running_thread]);
        t->state = ThreadState::SUSPEND;
        schedule.running_thread = -1;

        swapcontext(&(t->ctx), &(schedule.main));
    }
}

void uthread_body(schedule_t *ps) {
    int id = ps->running_thread;

    if (id != -1) {
        uthread_t *t = &(ps->threads[id]);

        std::invoke(t->func, t->arg);

        t->state = ThreadState::FREE;

        ps->running_thread = -1;
    }
}

int uthread_create(schedule_t &schedule, Func func, void *arg) {
    int id = 0;

    for (id = 0; id < schedule.max_index; ++id) {
        if (schedule.threads[id].state == ThreadState::FREE) {
            break;
        }
    }

    if (id == schedule.max_index) {
        schedule.max_index++;
    }

    uthread_t *t = &(schedule.threads[id]);

    t->state = ThreadState::RUNNABLE;
    t->func = std::move(func);
    t->arg = arg;

    getcontext(&(t->ctx));

    t->ctx.uc_stack.ss_sp = t->stack;
    t->ctx.uc_stack.ss_size = DEFAULT_STACK_SZIE;
    t->ctx.uc_stack.ss_flags = 0;
    t->ctx.uc_link = &(schedule.main);
    schedule.running_thread = id;

    makecontext(&(t->ctx), reinterpret_cast<void (*)(void)>(uthread_body), 1, &schedule);
    swapcontext(&(schedule.main), &(t->ctx));

    return id;
}

int schedule_finished(const schedule_t &schedule) {
    if (schedule.running_thread != -1) {
        return 0;
    } else {
        for (int i = 0; i < schedule.max_index; ++i) {
            if (schedule.threads[i].state != ThreadState::FREE) {
                return 0;
            }
        }
    }

    return 1;
}