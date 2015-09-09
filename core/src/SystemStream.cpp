/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <sys/types.h>
#include <sys/ioctl.h> // ioctl
#include <sys/uio.h> // readv
#include <errno.h>
#include <string.h>
#include <unistd.h> // read, write, select, sysconf
#include <fcntl.h> // fcntl
#include <math.h> // modf
#include <flux/exceptions>
#include <flux/SystemStream>

namespace flux {

SystemStream::SystemStream(int fd, bool iov)
    : fd_(fd),
      iov_(iov),
      iovMax_(0)
{}

SystemStream::~SystemStream()
{
    if (::close(fd_) == -1)
        FLUX_SYSTEM_DEBUG_ERROR(errno);
}

int SystemStream::fd() const { return fd_; }

bool SystemStream::isatty() const
{
    return ::isatty(fd_);
}

bool SystemStream::readyRead(double interval) const
{
    fd_set set;
    FD_ZERO(&set);
    FD_SET(fd_, &set);
    struct timeval tv, *to = 0;
    if (interval != inf) {
        if (interval < 0) interval = 0;
        to = &tv;
        double sec = 0;
        to->tv_usec = modf(interval, &sec) * 1e6;
        to->tv_sec = sec;
    }
    int ret = ::select(fd_ + 1, &set, 0, 0, to);
    if (ret == -1) FLUX_SYSTEM_DEBUG_ERROR(errno);
    return (ret > 0);
}

int SystemStream::read(ByteArray *data)
{
    ssize_t ret = 0;
    while (true) {
        ret = ::read(fd_, data->bytes(), data->count());
        if (ret == -1) {
            if (errno == EWOULDBLOCK) throw Timeout();
            if (errno == ECONNRESET) throw ConnectionResetByPeer();
            FLUX_SYSTEM_DEBUG_ERROR(errno);
        }
        break;
    }
    return ret;
}

void SystemStream::write(const ByteArray *data)
{
    const unsigned char *p = data->bytes();

    for (int n = data->count(); n > 0;)
    {
        ssize_t ret = ::write(fd_, p, n);
        if (ret == -1) {
            if (errno == EWOULDBLOCK) throw Timeout();
            if (errno == ECONNRESET) throw ConnectionResetByPeer();
            if (errno == EPIPE) throw ConnectionResetByPeer(); // FIXME: inprecise
            FLUX_SYSTEM_DEBUG_ERROR(errno);
        }
        p += ret;
        n -= ret;
    }
}

void SystemStream::write(const StringList *parts)
{
    if (!iov_) {
        write(parts->join());
        return;
    }
    int n = parts->count();
    if (n <= 0) return;
    Ref< Array<struct iovec> > iov = Array<struct iovec>::create(n);
    for (int i = 0; i < n; ++i) {
        ByteArray *part = parts->at(i);
        iov->at(i).iov_base = part->bytes();
        iov->at(i).iov_len = part->count();
    }
    if (iovMax_ == 0) iovMax_ = sysconf(_SC_IOV_MAX);
    for (int i = 0; i < iov->count();) {
        int n = iov->count() - i;
        if (n > iovMax_) n = iovMax_;
        ssize_t ret = ::writev(fd_, iov->constData() + i, n);
        if (ret == -1) {
            if (errno == EWOULDBLOCK) throw Timeout();
            if (errno == ECONNRESET) throw ConnectionResetByPeer();
            if (errno == EPIPE) throw ConnectionResetByPeer(); // FIXME: inprecise
            FLUX_SYSTEM_DEBUG_ERROR(errno);
        }
        i += n;
    }
}

void SystemStream::closeOnExec()
{
    if (::fcntl(fd_, F_SETFD, FD_CLOEXEC) == -1)
        FLUX_SYSTEM_DEBUG_ERROR(errno);
}

int SystemStream::ioctl(int request, void *arg)
{
    int value = ::ioctl(fd_, request, arg);
    if (value == -1)
        FLUX_SYSTEM_DEBUG_ERROR(errno);
    return value;
}

void SystemStream::duplicateTo(SystemStream *other)
{
    if (::dup2(fd_, other->fd_) == -1)
        FLUX_SYSTEM_DEBUG_ERROR(errno);
}

} // namespace flux
