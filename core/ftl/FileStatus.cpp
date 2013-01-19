 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */

#include <errno.h>
#include <sys/time.h> // futimes, utimes
#include "File.hpp"
#include "FileStatus.hpp"

namespace ftl
{

FileStatus::FileStatus(int fd)
	: fd_(fd)
{
	if (fd == -1) {
		mem::clr(static_cast<StructStat*>(this), sizeof(StructStat));
		exists_ = false;
		return;
	}
	exists_ = update();
}

FileStatus::FileStatus(String path, bool resolve)
	: fd_(-1),
	  path_(path),
	  resolve_(resolve)
{
	exists_ = update();
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

bool FileStatus::update()
{
	StructStat *buf = static_cast<StructStat *>(this);
	mem::clr(buf, sizeof(StructStat));
	int ret = (fd_ != -1) ? ::fstat(fd_, buf) : (resolve_ ? ::stat(path_, buf) : ::lstat(path_, buf));
	if (ret == -1) {
		if ((errno == ENOENT) || (errno == ENOTDIR)) {
			exists_ = false;
			return false;
		}
		FTL_SYSTEM_EXCEPTION;
	}
	exists_ = true;
	return true;
}

} // namespace ftl
