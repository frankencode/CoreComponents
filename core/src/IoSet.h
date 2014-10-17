/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_IOSET_H
#define FLUX_IOSET_H

#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#include <flux/SystemStream>

namespace flux {

class IoSet: public Object
{
public:
    inline static Ref<IoSet> create() { return new IoSet; }

    inline void insert(SystemStream *stream) { FD_SET(updateBitSize(stream->fd()), &bitSet_); }
    inline void remove(SystemStream *stream) { FD_CLR(updateBitSize(stream->fd()), &bitSet_); }
    inline bool contains(SystemStream *stream) { return FD_ISSET(updateBitSize(stream->fd()), &bitSet_); }
    inline void clear() { bitSize_ = 0; FD_ZERO(&bitSet_); }

    inline fd_set *bitSet() { return &bitSet_; }
    inline int bitSize() const { return bitSize_; }

private:
    IoSet(): bitSize_(0) { FD_ZERO(&bitSet_); }
    inline int updateBitSize(int fd) {
        if (fd >= bitSize_) bitSize_ = fd + 1;
        return fd;
    }
    fd_set bitSet_;
    int bitSize_;
};

} // namespace flux

#endif // FLUX_IOSET_H
