/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/ReadWriteLock>

namespace cc {

ReadWriteLock::ReadWriteLock()
{
    int ret = pthread_rwlock_init(&lock_, nullptr);
    if (ret != 0) throw SystemError{ret};
}

ReadWriteLock::~ReadWriteLock()
{
    pthread_rwlock_destroy(&lock_);
}

void ReadWriteLock::readAcquire()
{
    int ret = pthread_rwlock_rdlock(&lock_);
    if (ret != 0) throw SystemError{ret};
}

void ReadWriteLock::writeAcquire()
{
    int ret = pthread_rwlock_wrlock(&lock_);
    if (ret != 0) throw SystemError{ret};
}

void ReadWriteLock::release()
{
    int ret = pthread_rwlock_unlock(&lock_);
    if (ret != 0) throw SystemError{ret};
}

} // namespace cc
