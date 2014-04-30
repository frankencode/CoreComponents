/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <errno.h>
#include "Exception.h"
#include "FileLock.h"

namespace flux
{

FileLock::FileLock(File *file, int type, off_t start, off_t length)
	: fd_(file->fd())
{
	memclr(static_cast<FLockStruct*>(this), sizeof(FLockStruct));
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
			FLUX_SYSTEM_EXCEPTION;
	return success;
}

void FileLock::acquire()
{
	while (true) {
		if (::fcntl(fd_, F_SETLKW, static_cast<FLockStruct*>(this)) == -1) {
			if (errno == EINTR) throw Interrupt();
			FLUX_SYSTEM_EXCEPTION;
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
		FLUX_SYSTEM_EXCEPTION;
}

} // namespace flux
