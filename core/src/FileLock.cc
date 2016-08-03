/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <errno.h>
#include <cc/exceptions>
#include <cc/FileLock>

namespace cc {

FileLock::FileLock(File *file, Type type, off_t start, off_t length):
    fd_(file->fd())
{
    memclr(static_cast<FLockStruct *>(this), sizeof(FLockStruct));
    FLockStruct::l_type = int(type);
    FLockStruct::l_start = start;
    FLockStruct::l_len = length;
    FLockStruct::l_whence = SEEK_SET;
}

bool FileLock::tryAcquire()
{
    bool success = (::fcntl(fd_, F_SETLK, static_cast<FLockStruct*>(this)) != -1);
    if (!success)
        if ((errno != EACCES) && (errno != EAGAIN))
            CC_SYSTEM_DEBUG_ERROR(errno);
    return success;
}

void FileLock::acquire()
{
    if (::fcntl(fd_, F_SETLKW, static_cast<FLockStruct*>(this)) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

void FileLock::release()
{
    struct flock h = *static_cast<FLockStruct*>(this);
    h.l_type = F_UNLCK;
    if (::fcntl(fd_, F_SETLK, &h) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

} // namespace cc
