#include <glog/logging.h>
#include <gtest/gtest.h>
#include "coroutine/uthread.h"

class UThreadTest : public ::testing::Test {
public:
    void SetUp() override {}

    void TearDown() override {}

};

void uthread_func1(void *arg) {
    LOG(INFO) << "1";
    LOG(INFO) << "11";
    LOG(INFO) << "111";
    LOG(INFO) << "1111";
}

void uthread_func2(void *arg) {
    LOG(INFO) << "22";
    LOG(INFO) << "22";
    uthread_yield(*(schedule_t *) arg);
    LOG(INFO) << "22";
    LOG(INFO) << "22";
}

void uthread_func3(void *arg) {
    LOG(INFO) << "3333";
    LOG(INFO) << "3333";
    uthread_yield(*(schedule_t *) arg);
    LOG(INFO) << "3333";
    LOG(INFO) << "3333";
}

void context_test() {
    char stack[1024 * 128];
    ucontext_t uc1, ucmain;

    getcontext(&uc1);
    uc1.uc_stack.ss_sp = stack;
    uc1.uc_stack.ss_size = 1024 * 128;
    uc1.uc_stack.ss_flags = 0;
    uc1.uc_link = &ucmain;
    makecontext(&uc1, reinterpret_cast<void (*)(void)>(uthread_func1), 0);
    swapcontext(&ucmain, &uc1);
    LOG(INFO) << "---- main ----";
}

void schedule_test() {
    schedule_t s;
    LOG(INFO) <<"Schedule init ... ";
    int id1 = uthread_create(s, uthread_func3, &s);
    LOG(INFO) << "uthread3 yileding ...";
    int id2 = uthread_create(s, uthread_func2, &s);
    LOG(INFO) << "uthread2 yileding ...";

    while (!schedule_finished(s)) {
        LOG(INFO) << "uthread3 resuming ...";
        uthread_resume(s, id2);
        LOG(INFO) << "uthread2 resuming ...";
        uthread_resume(s, id1);
    }
    LOG(INFO) << "Schedule over ...";
}

TEST_F(UThreadTest, ContextTest) {
    context_test();
}

TEST_F(UThreadTest, ScheduleTest) {
    schedule_test();
}