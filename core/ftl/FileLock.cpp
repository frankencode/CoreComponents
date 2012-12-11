/*
 * FileLock.cpp -- advisory file locks
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include <errno.h>
#include "FileLock.hpp"

namespace ftl
{

FileLock::FileLock(Ref<File> file, int type, off_t start, off_t length)
	: fd_(file->fd())
{
	mem::clr(static_cast<FLockStruct*>(this), sizeof(FLockStruct));
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
			FTL_SYSTEM_EXCEPTION;
	return success;
}

void FileLock::acquire()
{
	while (true) {
		if (::fcntl(fd_, F_SETLKW, static_cast<FLockStruct*>(this)) == -1) {
			if (errno != EINTR)
				FTL_SYSTEM_EXCEPTION;
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
		FTL_SYSTEM_EXCEPTION;
}

} // namespace ftl
