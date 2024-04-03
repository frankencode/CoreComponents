/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/FileLock>
#include <cstring>

namespace cc {

FileLock::FileLock(const File &file, FileContention type, long long start, long long length, Seek whence):
    file_{file}
{
    std::memset(&lock_, 0, sizeof(lock_));
    lock_.l_type = +type;
    lock_.l_start = start;
    lock_.l_len = length;
    lock_.l_whence = +whence;
}

bool FileLock::tryAcquire()
{
    bool success = (::fcntl(file_.fd(), F_SETLK, &lock_) != -1);
    if (!success) {
        if ((errno != EACCES) && (errno != EAGAIN))
            CC_SYSTEM_DEBUG_ERROR(errno);
    }
    return success;
}

void FileLock::acquire()
{
    if (::fcntl(file_.fd(), F_SETLKW, &lock_) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

void FileLock::release()
{
    lock_.l_type = F_UNLCK;
    if (::fcntl(file_.fd(), F_SETLK, &lock_) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

} // namespace cc
