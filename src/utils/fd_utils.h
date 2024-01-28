#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

// RAII file descriptor.
//
// Example:
//    fd_guard fd1(open(...));
//    if (fd1 < 0) {
//        printf("Fail to open\n");
//        return -1;
//    }
//    if (another-error-happened) {
//        printf("Fail to do sth\n");
//        return -1;   // *** closing fd1 automatically ***
//    }

class fd_guard {
private:
    int fd_;
public:
    fd_guard() : fd_(-1) {}

    ~fd_guard() {
        if (fd_ >= 0) {
            ::close(fd_);
            fd_ = -1;
        }
    }

    fd_guard(const fd_guard &) = delete;

    fd_guard &operator=(const fd_guard &) = delete;

    fd_guard(fd_guard &&other) {
        fd_ = other.fd_;
        other.fd_ = -1;
    }

    fd_guard &operator=(fd_guard &&other) {
        if (this != &other) {
            fd_ = other.fd_;
            other.fd_ = -1;
        }
        return *this;
    }

    // Close current fd and replace with another fd
    void reset(int fd) {
        if (fd_ >= 0) {
            ::close(fd_);
            fd_ = -1;
        }
        fd_ = fd;
    }

    // Set internal fd to -1 and return the value before set.
    int release() {
        const int prevfd = fd_;
        fd_ = -1;
        return prevfd;
    }

    operator int() const { return fd_; }
};

// Make file descriptor |fd| non-blocking
// Returns 0 on success, -1 otherwise and errno is set (by fcntl)
int make_non_blocking(int fd){
    const int flag = fcntl(fd, F_GETFL, 0);
    if (flag < 0){
        return flag;
    }
    if (flag & O_NONBLOCK){
        return 0;
    }

    return fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}

// Make file descriptor |fd| blocking
// Returns 0 on success, -1 otherwise and errno is set (by fcntl)
int make_blocking(int fd){
    const int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        return flags;
    }
    if (flags & O_NONBLOCK) {
        return fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
    }
    return 0;
}

// Make file descriptor |fd| automatically closed during exec()
// Returns 0 on success, -1 when error and errno is set (by fcntl)
int make_close_on_exec(int fd){
    return fcntl(fd, F_SETFD, FD_CLOEXEC);
}

// Disable nagling on file descriptor |socket|.
// Returns 0 on success, -1 when error and errno is set (by setsockopt)
int make_no_delay(int socket){
    int flag = 1;
    return setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));
}