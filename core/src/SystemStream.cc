/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <sys/types.h>
#include <sys/uio.h> // readv
#include <sys/socket.h> // socketpair
#include <errno.h>
#include <string.h>
#include <unistd.h> // read, write, select, sysconf
#include <fcntl.h> // fcntl
#include <math.h> // modf
#ifndef NDEBUG
#include <cc/check>
#endif
#include <cc/exceptions>
#include <cc/SystemIo>
#include <cc/Format>
#include <cc/SystemStream>

namespace cc {

Ref<SystemStream> SystemStream::duplicate(int fd)
{
    int fd2 = ::dup(fd);
    if (fd == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    return new SystemStream(fd2);
}

Ref<SystemStream> SystemStream::duplicate(SystemStream *other)
{
    return duplicate(other->fd_);
}

void SystemStream::connect(Ref<SystemStream> *first, Ref<SystemStream> *second)
{
    int fd[2];
    fd[0] = 0;
    fd[1] = 0;
    if (::socketpair(AF_LOCAL, SOCK_STREAM, 0, fd) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
    *first = new SystemStream(fd[0]);
    *second = new SystemStream(fd[1]);
}

SystemStream::SystemStream(int fd):
    fd_(fd),
    scatterLimit_(1<<14)
{}

SystemStream::~SystemStream()
{
    if (fd_ >= 0) {
        #ifndef NDEBUG
        int ret =
        #endif
        ::close(fd_);
        #ifndef NDEBUG
        check(ret != -1);
        #endif
    }
}

int SystemStream::fd() const { return fd_; }

bool SystemStream::isatty() const
{
    return ::isatty(fd_);
}

int SystemStream::read(ByteArray *data)
{
    return SystemIo::read(fd_, data->bytes(), data->count());
}

void SystemStream::write(const ByteArray *data)
{
    SystemIo::write(fd_, data->bytes(), data->count());
}

void SystemStream::write(const StringList *parts)
{
    const int n = parts->count();

    if (n <= 0) return;

    if (scatterLimit_ <= 0) {
        write(parts->join());
        return;
    }

    {
        int total = 0;
        for (int i = 0; i < n; ++i)
            total += parts->at(i)->count();

        if (total <= scatterLimit_) {
            write(parts->join());
            return;
        }
    }

    typedef struct iovec IoVector;
    struct IoGuard {
        IoGuard(int n): iov_(new IoVector[n]) {}
        ~IoGuard() { delete[] iov_; }
        IoVector *iov_;
    };

    IoGuard guard(n);
    IoVector *iov = guard.iov_;

    for (int i = 0; i < n; ++i) {
        const ByteArray *part = parts->at(i);
        iov[i].iov_base = (void *)part->bytes();
        iov[i].iov_len = part->count();
    }

    SystemIo::writev(fd_, iov, n);
}

void SystemStream::write(const Format &data)
{
    write(Format::toStringList(data));
}

bool SystemStream::waitFor(int events, int interval_ms)
{
    return SystemIo::poll(fd_, events, interval_ms);
}

void SystemStream::shutdown(ShutdownType type)
{
    if (::shutdown(fd_, type) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

void SystemStream::duplicateTo(SystemStream *other)
{
    if (::dup2(fd_, other->fd_) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

int SystemStream::ioctl(int request, void *arg)
{
    return SystemIo::ioctl(fd_, request, arg);
}

} // namespace cc
