/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/IoStream>
#include <sys/types.h>
#include <sys/uio.h> // readv, writev
#include <sys/ioctl.h> // ioctl
#include <sys/socket.h> // socketpair, shutdown, SHUT_*
#include <termios.h> // struct termios, ECHO
#include <unistd.h> // read, write, select, sysconf, isatty
#include <poll.h> // poll, POLLIN, POLLOUT

namespace cc {

IoStream::State::~State()
{
    if (fd_ >= 3) ::close(fd_);
}

bool IoStream::State::wait(IoEvent event, int timeout)
{
    struct pollfd fds;
    fds.fd = fd_;
    fds.events = 0;
    if (event & IoEvent::ReadyRead) fds.events |= POLLIN;
    if (event & IoEvent::ReadyWrite) fds.events |= POLLOUT;

    if (timeout < 0) timeout = -1;
    int ret = -1;
    do ret = ::poll(&fds, 1, timeout);
    while (ret == -1 && errno == EINTR);
    if (ret == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    assert(ret == 0 || ret == 1);
    assert(timeout != -1 || ret == 1);

    return ret == 1;
}

long IoStream::State::read(Out<Bytes> buffer, long maxFill)
{
    long n = (maxFill < 0 || buffer().count() < maxFill) ? buffer().count() : maxFill;
    long m = -1;
    do m = ::read(fd_, buffer(), n);
    while (m == -1 && errno == EINTR);
    if (m == -1) {
        if (errno == EWOULDBLOCK) throw Timeout{};
        if (errno == ECONNRESET || errno == EPIPE) throw InputExhaustion{};
        #if defined __CYGWIN__ || defined __CYGWIN32__
        if (errno == ECONNABORTED) return 0;
        #endif
        CC_SYSTEM_DEBUG_ERROR(errno);
    }
    return m;
}

void IoStream::State::write(const Bytes &buffer, long fill)
{
    const uint8_t *p = buffer.bytes();
    long n = (0 < fill && fill < buffer.count()) ? fill : buffer.count();

    while (n > 0) {
        long m = -1;
        do m = ::write(fd_, p, n);
        while (m == -1 && errno == EINTR);
        if (m == -1) {
            if (errno == EWOULDBLOCK) throw Timeout{};
            if (errno == ECONNRESET || errno == EPIPE) throw OutputExhaustion{};
            CC_SYSTEM_DEBUG_ERROR(errno);
        }
        p += m;
        n -= m;
    }
}

void IoStream::State::write(const List<Bytes> &buffers)
{
    const int n = buffers.count();

    if (n <= 0) return;

    if (scatterLimit_ <= 0) {
        write(Bytes{buffers});
        return;
    }

    {
        long total = 0;
        for (long i = 0; i < n; ++i)
            total += buffers.at(i).count();

        if (total <= scatterLimit_) {
            write(Bytes{buffers});
            return;
        }
    }

    using IoItem = struct iovec;

    struct IoVector {
        IoVector(long n): items{new IoItem[n]} {}
        ~IoVector() { delete[] items; }
        IoItem *items;
    };

    IoVector vector{n};
    IoItem *iov = vector.items;

    for(auto pos = buffers.head(); pos; ++pos) {
        const Bytes &buffer = buffers.at(pos);
        iov[+pos].iov_base = const_cast<uint8_t *>(buffer.items());
        iov[+pos].iov_len = buffer.count();
    }

    const int iovMax = ::sysconf(_SC_IOV_MAX);

    for (int i = 0; i < n;) {
        int m = n - i;
        if (m > iovMax) m = iovMax;
        ssize_t ret = -1;
        do ret = ::writev(fd_, iov + i, m);
        while (ret == -1 && errno == EINTR);
        if (ret == -1) {
            if (errno == EWOULDBLOCK) throw Timeout{};
            if (errno == ECONNRESET || errno == EPIPE) throw OutputExhaustion{};
            CC_SYSTEM_DEBUG_ERROR(errno);
        }
        i += m;
    }
}

IoStream &IoStream::input()
{
    static thread_local IoStream stream{0};
    return stream;
}

IoStream &IoStream::output()
{
    static thread_local IoStream stream{1};
    return stream;
}

IoStream &IoStream::error()
{
    static thread_local IoStream stream{2};
    return stream;
}

void IoStream::pair(Out<IoStream> a, Out<IoStream> b)
{
    int fd[2] = { 0, 0 };

    if (::socketpair(AF_LOCAL, SOCK_STREAM|SOCK_CLOEXEC, 0, fd) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);

    a = IoStream{fd[0]};
    b = IoStream{fd[1]};
}

IoStream::IoStream(int fd):
    Stream{new IoStream::State{fd}}
{}

void IoStream::shutdown(IoShutdown mode)
{
    if (fd() >= 0) {
        int how = 0;
        if (mode == IoShutdown::Read) how |= SHUT_RD;
        if (mode == IoShutdown::Write) how |= SHUT_WR;
        if (mode == IoShutdown::Full) how |= SHUT_RDWR;
        if (::shutdown(fd(), how) == -1)
            CC_SYSTEM_DEBUG_ERROR(errno);
    }
}

void IoStream::duplicateTo(IoStream &other)
{
    if (::dup2(fd(), other.me().fd_) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

IoStream IoStream::duplicate() const
{
    int fd2 = ::dup(fd());
    if (fd2 == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    return IoStream{fd2};
}

bool IoStream::isInteractive() const
{
    return ::isatty(fd());
}

void IoStream::echo(bool on)
{
    struct termios settings;

    if (::tcgetattr(fd(), &settings) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);

    if (((settings.c_lflag & ECHO) != 0) == on) return;

    if (on)
        settings.c_lflag |= ECHO;
    else
        settings.c_lflag &= ~ECHO;

    if (::tcsetattr(fd(), TCSAFLUSH, &settings) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

int IoStream::ioctl(int request, void *arg)
{
    int value = ::ioctl(fd(), request, arg);

    if (value == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);

    return value;
}

} // namespace cc
