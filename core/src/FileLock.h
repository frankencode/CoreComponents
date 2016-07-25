/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <unistd.h>
#include <fcntl.h>
#include <cc/File>

namespace cc {

typedef struct flock FLockStruct;

/** \brief Advisory file locks
  * \see Guard
  */
class FileLock: public FLockStruct, public Object
{
public:
    enum Type { ReadLock = F_RDLCK, WriteLock = F_WRLCK };

    static Ref<FileLock> create(File *file, int type, off_t start = 0, off_t length = 0) {
        return new FileLock(file, type, start, length);
    }

    bool tryAcquire();
    void acquire();
    void release();

private:
    FileLock(File *file, int type, off_t start = 0, off_t length = 0);

    int fd_;
};

} // namespace cc

