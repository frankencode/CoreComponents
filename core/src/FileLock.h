/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_FILELOCK_H
#define FLUX_FILELOCK_H

#include <unistd.h>
#include <fcntl.h>
#include <flux/File>

namespace flux {

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

} // namespace flux

#endif // FLUX_FILELOCK_H
