/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <errno.h>
#include <flux/exceptions>
#include <flux/FileLock>

namespace flux {

FileLock::FileLock(File *file, int type, off_t start, off_t length)
	: fd_(file->fd())
{
	memclr(static_cast<FLockStruct *>(this), sizeof(FLockStruct));
	FLockStruct::l_type = type;
	FLockStruct::l_start = start;
	FLockStruct::l_len = length;
	FLockStruct::l_whence = SEEK_SET;
}

bool FileLock::tryAcquire()
{
	bool success = (::fcntl(fd_, F_SETLK, static_cast<FLockStruct*>(this)) != -1);
	if (!success)
		if ((errno != EACCES) && (errno != EAGAIN))
			FLUX_SYSTEM_DEBUG_ERROR(errno);
	return success;
}

void FileLock::acquire()
{
	while (true) {
		if (::fcntl(fd_, F_SETLKW, static_cast<FLockStruct*>(this)) == -1) {
			if (errno == EINTR) throw Interrupt();
			FLUX_SYSTEM_DEBUG_ERROR(errno);
		}
		else
			break;
	}
}

void FileLock::release()
{
	struct flock h = *static_cast<FLockStruct*>(this);
	h.l_type = F_UNLCK;
	if (::fcntl(fd_, F_SETLK, &h) == -1)
		FLUX_SYSTEM_DEBUG_ERROR(errno);
}

} // namespace flux
