/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/FileLock>
#include <cc/exceptions>
#include <errno.h>

namespace cc {

FileLock::Instance::Instance(File &file, Type type, off_t start, off_t length):
    fd_{file->fd()}
{
    memclr(static_cast<FLockStruct *>(this), sizeof(FLockStruct));
    FLockStruct::l_type = static_cast<int>(type);
    FLockStruct::l_start = start;
    FLockStruct::l_len = length;
    FLockStruct::l_whence = SEEK_SET;
}

bool FileLock::Instance::tryAcquire()
{
    bool success = (::fcntl(fd_, F_SETLK, static_cast<FLockStruct*>(this)) != -1);
    if (!success)
        if ((errno != EACCES) && (errno != EAGAIN))
            CC_SYSTEM_DEBUG_ERROR(errno);
    return success;
}

void FileLock::Instance::acquire()
{
    if (::fcntl(fd_, F_SETLKW, static_cast<FLockStruct*>(this)) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

void FileLock::Instance::release()
{
    struct flock h = *static_cast<FLockStruct*>(this);
    h.l_type = F_UNLCK;
    if (::fcntl(fd_, F_SETLK, &h) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

} // namespace cc
