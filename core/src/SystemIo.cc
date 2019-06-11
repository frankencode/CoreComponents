/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <sys/uio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cc/assert>
#include <cc/exceptions>
#include <cc/Singleton>
#include <cc/SystemIo>

namespace cc {

SystemIo::SystemIo():
    iovMax_(::sysconf(_SC_IOV_MAX))
{}

bool SystemIo::poll(int fd, int events, int timeout_ms)
{
    struct pollfd fds;
    fds.fd = fd;
    fds.events = events;

    if (timeout_ms < 0) timeout_ms = -1;
    int ret = -1;
    do ret = ::poll(&fds, 1, timeout_ms);
    while (ret == -1 && errno == EINTR);
    if (ret == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    CC_ASSERT(ret == 0 || ret == 1);
    CC_ASSERT(timeout_ms != -1 || ret == 1);

    return ret == 1;
}

ssize_t SystemIo::read(int fd, void *buf, size_t count)
{
    ssize_t ret = -1;
    do ret = ::read(fd, buf, count);
    while (ret == -1 && errno == EINTR);
    SystemIo::checkErrors(ret);
    return ret;
}

void SystemIo::write(int fd, const void *buf, size_t count)
{
    const char *p = (const char *)buf;

    for (ssize_t n = count; n > 0;)
    {
        ssize_t ret = -1;
        do ret = ::write(fd, p, n);
        while (ret == -1 && errno == EINTR);
        SystemIo::checkErrors(ret);
        p += ret;
        n -= ret;
    }
}

void SystemIo::writev(int fd, const struct iovec *iov, int iovcnt)
{
    const int iovMax = SystemIo::instance()->iovMax();
    for (int i = 0; i < iovcnt;) {
        int m = iovcnt - i;
        if (m > iovMax) m = iovMax;
        ssize_t ret = -1;
        do ret = ::writev(fd, iov + i, m);
        while (ret == -1 && errno == EINTR);
        SystemIo::checkErrors(ret);
        i += m;
    }
}

int SystemIo::ioctl(int fd, int request, void *arg)
{
    int value = ::ioctl(fd, request, arg);
    if (value == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
    return value;
}

void SystemIo::checkErrors(int ret)
{
    if (ret == -1) {
        if (errno == EWOULDBLOCK) throw Timeout{};
        if (errno == ECONNRESET || errno == EPIPE) throw ConnectionResetByPeer{};
        CC_SYSTEM_DEBUG_ERROR(errno);
    }
}

SystemIo *SystemIo::instance()
{
    return Singleton<SystemIo>::instance();
}

} // namespace cc
