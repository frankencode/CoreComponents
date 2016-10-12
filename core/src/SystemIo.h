/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <sys/types.h>
#include <sys/poll.h>
#include <cc/Object>

namespace cc {

template<class> class Singleton;

class SystemIo: public Object {
public:
    enum WaitEvent {
        ReadyRead = POLLIN,
        ReadyWrite = POLLOUT,
        ReadyAccept = ReadyRead
    };

    static bool poll(int fd, int events, int interval_ms = -1);

    static ssize_t read(int fd, void *buf, size_t count);
    static void write(int fd, const void *buf, size_t count);
    static void writev(int fd, const struct iovec *iov, int iovcnt);

    inline int iovMax() const { return iovMax_; }

    static int ioctl(int fd, int request, void *arg);

protected:
    friend class Singleton<SystemIo>;

    SystemIo();

    static void checkErrors(int ret);

    static SystemIo *instance();

    int iovMax_;
};

} // namespace cc
