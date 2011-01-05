/*
 * FileLock.cpp -- advisory file locks
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include <errno.h>
#include "FileLock.hpp"

namespace ftl
{

FileLock::FileLock(Ref<File> file, int type, off_t start, off_t length)
	: fd_(file->fd())
{
	mem::clr(this, sizeof(FLockStruct));
	FLockStruct::l_type = type;
	FLockStruct::l_start = start;
	FLockStruct::l_len = length;
	FLockStruct::l_whence = SEEK_SET;
	if (!file->isOpen())
		FTL_THROW(StreamSemanticException, "File needs to be opened, before it can be locked.");
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
