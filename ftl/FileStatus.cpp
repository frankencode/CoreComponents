/*
 * FileStatus.cpp -- file type, size, ownership, etc.
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include <errno.h>
#include "FileStatus.hpp"

namespace ftl
{

FileStatus::FileStatus(int fd)
	: fd_(fd)
{
	update();
}

FileStatus::FileStatus(Ref<SystemStream> stream)
	: fd_(stream->fd())
{
	update();
}

FileStatus::FileStatus(String path, bool followSymbolicLink)
	: fd_(-1),
	  path_(path)
{
	update(0, followSymbolicLink);
}

void FileStatus::setTimes(Time lastAccess, Time lastModified)
{
	struct timeval tv[2];
	tv[0].tv_sec = lastAccess.sec();
	tv[0].tv_usec = lastAccess.usec();
	tv[1].tv_sec = lastModified.sec();
	tv[1].tv_usec = lastModified.usec();
	int ret = (fd_ != -1) ? ::futimes(fd_, tv) : ::utimes(path_, tv);
	if(ret == -1)
		FTL_SYSTEM_EXCEPTION;
}

void FileStatus::update(bool* exists, bool followSymbolicLink)
{
	int ret = (fd_ != -1) ? ::fstat(fd_, this) : (followSymbolicLink ? ::stat(path_, this) : ::lstat(path_, this));
	if (ret == -1) {
		if (exists) {
			if ((errno == ENOENT) || (errno == ENOTDIR)) {
				*exists = false;
				return;
			}
		}
		FTL_SYSTEM_EXCEPTION;
	}
}

} // namespace ftl
