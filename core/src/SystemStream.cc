/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/SystemStream>
#include <cc/SystemIo>
#include <cc/Format>
#include <cc/exceptions>
#ifndef NDEBUG
#include <cc/check>
#endif
#include <sys/types.h>
#include <sys/uio.h> // readv, writev
#include <sys/socket.h> // socketpair
#include <errno.h>
#include <string.h>
#include <termios.h> // ECHO
#include <unistd.h> // read, write, select, sysconf
#include <fcntl.h> // fcntl
#include <math.h> // modf

namespace cc {

SystemStream::Instance::Instance(int fd):
    fd_{fd},
    scatterLimit_{1<<14}
{}

SystemStream::Instance::~Instance()
{
    if (fd_ >= 0) ::close(fd_);
}

int SystemStream::Instance::fd() const { return fd_; }

bool SystemStream::Instance::isatty() const
{
    return ::isatty(fd_);
}

void SystemStream::Instance::echo(bool on)
{
    struct termios settings;

    if (::tcgetattr(fd_, &settings) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);

    if (((settings.c_lflag & ECHO) != 0) == on) return;

    if (on)
        settings.c_lflag |= ECHO;
    else
        settings.c_lflag &= ~ECHO;

    if (::tcsetattr(fd_, TCSAFLUSH, &settings) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

int SystemStream::Instance::read(CharArray *data)
{
    return SystemIo::read(fd_, data->bytes(), data->count());
}

void SystemStream::Instance::write(const CharArray *data)
{
    SystemIo::write(fd_, data->bytes(), data->count());
}

void SystemStream::Instance::write(const StringList &parts)
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
        IoGuard(int n): iov_{new IoVector[n]} {}
        ~IoGuard() { delete[] iov_; }
        IoVector *iov_;
    };

    IoGuard guard{n};
    IoVector *iov = guard.iov_;

    for (int i = 0; i < n; ++i) {
        const CharArray *part = parts->at(i);
        iov[i].iov_base = (void *)part->bytes();
        iov[i].iov_len = part->count();
    }

    SystemIo::writev(fd_, iov, n);
}

// \todo method should not be needed
void SystemStream::Instance::write(const Format &data)
{
    write(static_cast<const StringList &>(data));
}

bool SystemStream::Instance::waitFor(IoReady ready, int interval_ms)
{
    return SystemIo::poll(fd_, static_cast<int>(ready), interval_ms);
}

void SystemStream::Instance::shutdown(Shutdown type)
{
    if (::shutdown(fd_, static_cast<int>(type)) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

void SystemStream::Instance::duplicateTo(SystemStream &other)
{
    if (::dup2(fd_, other->fd_) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

int SystemStream::Instance::ioctl(int request, void *arg)
{
    return SystemIo::ioctl(fd_, request, arg);
}

void SystemStream::Instance::close()
{
    if (::close(fd_) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

void SystemStream::Instance::duplicate(const SystemStream &other)
{
    int fd2 = ::dup(other->fd_);
    if (fd2 == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    if (fd_ != -1) close();
    fd_ = fd2;
}

void SystemStream::Instance::connect(SystemStream &other)
{
    int fd[2] = { 0, 0 };
    if (::socketpair(AF_LOCAL, SOCK_STREAM|SOCK_CLOEXEC, 0, fd) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
    if (fd_ != -1) close();
    if (other->fd_ != -1) other->close();
    fd_ = fd[0];
    other->fd_ = fd[1];
}

} // namespace cc
