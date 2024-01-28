#include <gtest/gtest.h>
#include <unistd.h>
#include "utils/fd_utils.h" // include file with class definition

class FdGuardTest : public ::testing::Test {
protected:
    fd_guard guard;

    // Just produces a dummy file descriptor for testing
    int GetDummyDescriptor() {
        return ::dup(STDIN_FILENO);
    }
};

TEST_F(FdGuardTest, DefaultConstructor) {
    // default fd should be -1
    EXPECT_EQ(-1, static_cast<int>(guard));
}

TEST_F(FdGuardTest, MoveConstruction) {
    int desc = GetDummyDescriptor();
    fd_guard newGuard;
    newGuard.reset(desc);
    fd_guard movedGuard(std::move(newGuard));
    EXPECT_EQ(-1, static_cast<int>(newGuard));
    EXPECT_EQ(desc, static_cast<int>(movedGuard));
}

TEST_F(FdGuardTest, MoveAssignment) {
    int desc = GetDummyDescriptor();
    fd_guard newGuard;
    newGuard.reset(desc);

    fd_guard movedGuard;
    movedGuard = std::move(newGuard);

    EXPECT_EQ(-1, static_cast<int>(newGuard));
    EXPECT_EQ(desc, static_cast<int>(movedGuard));
}

TEST_F(FdGuardTest, Reset) {
    int desc = GetDummyDescriptor();
    guard.reset(desc);

    EXPECT_EQ(desc, static_cast<int>(guard));

    // Checking bare minimum functionality of reset, that is it replaces the current fd
    int newDesc = GetDummyDescriptor();
    guard.reset(newDesc);

    EXPECT_EQ(newDesc, static_cast<int>(guard));
}

TEST_F(FdGuardTest, Release) {
    int desc = GetDummyDescriptor();
    guard.reset(desc);

    EXPECT_EQ(desc, guard.release());
    EXPECT_EQ(-1, static_cast<int>(guard));
}

TEST(FcntlTest, NonBlockingTest) {
    // create a socket and set it to blocking mode
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    auto helper_fd_blocking_mode = [](int fd) {
        const int flags = fcntl(fd, F_GETFL, 0);
        return fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
    };
    helper_fd_blocking_mode(fd);

    // Make the fd non-blocking
    int non_blocking_status = make_non_blocking(fd);
    EXPECT_EQ(non_blocking_status, 0);

    // Check if the fd is non-blocking now
    const int flags = fcntl(fd, F_GETFL, 0);
    EXPECT_TRUE(flags & O_NONBLOCK);
}

TEST(FcntlTest, BlockingTest) {
    // create a socket and set it to non-blocking mode
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(fd, F_SETFL, O_NONBLOCK);

    // Make the fd blocking
    int blocking_status = make_blocking(fd);
    EXPECT_EQ(blocking_status, 0);

    // Check if the fd is blocking now
    const int flags = fcntl(fd, F_GETFL, 0);
    EXPECT_FALSE(flags & O_NONBLOCK);
}

TEST(FcntlTest, CloseOnExecTest) {
    // create a socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    // Enable the close-on-exec flag
    int close_on_exec_status = make_close_on_exec(fd);
    EXPECT_EQ(close_on_exec_status, 0);

    // Verify flag is set
    const int flags = fcntl(fd, F_GETFD, 0);
    EXPECT_TRUE(flags & FD_CLOEXEC);
}

TEST(SocketTest, NoDelayTest) {
    // create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    // Enable the TCP_NODELAY flag
    int no_delay_status = make_no_delay(sock);
    EXPECT_EQ(no_delay_status, 0);

    // Verify flag is set
    int flag = 0;
    socklen_t len = sizeof(int);
    int result = getsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &flag, &len);
    EXPECT_EQ(result, 0);
    EXPECT_EQ(flag, 1);
}