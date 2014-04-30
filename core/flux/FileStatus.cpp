/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <sys/time.h> // futimes, utimes
#include <errno.h>
#include <math.h>
#include "File.h"
#include "Exception.h"
#include "FileStatus.h"

namespace flux
{

FileStatus::FileStatus(int fd)
	: fd_(fd)
{
	if (fd == -1) {
		memclr(static_cast<StructStat*>(this), sizeof(StructStat));
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

void FileStatus::setTimes(double lastAccess, double lastModified)
{
	struct timeval tv[2];
	double sec;
	tv[0].tv_usec = modf(lastAccess, &sec) * 1e6;
	tv[0].tv_sec = sec;
	tv[1].tv_usec = modf(lastModified, &sec) * 1e6;
	tv[1].tv_sec = sec;
	int ret = (fd_ != -1) ? ::futimes(fd_, tv) : ::utimes(path_, tv);
	if(ret == -1)
		FLUX_SYSTEM_EXCEPTION;
}

bool FileStatus::update()
{
	StructStat *buf = static_cast<StructStat *>(this);
	memclr(buf, sizeof(StructStat));
	int ret = (fd_ != -1) ? ::fstat(fd_, buf) : (resolve_ ? ::stat(path_, buf) : ::lstat(path_, buf));
	if (ret == -1) {
		if ((errno == ENOENT) || (errno == ENOTDIR)) {
			exists_ = false;
			return false;
		}
		FLUX_SYSTEM_EXCEPTION;
	}
	exists_ = true;
	return true;
}

} // namespace flux
