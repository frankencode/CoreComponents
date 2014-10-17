/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <sys/time.h> // futimes, utimes
#include <errno.h>
#include <math.h>
#include <flux/File>
#include <flux/exceptions>
#include <flux/FileStatus>

namespace flux {

FileStatus::FileStatus(int fd)
	: fd_(fd)
{
	if (fd == -1) {
		memclr(static_cast<StructStat *>(this), sizeof(StructStat));
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
	if(ret == -1) FLUX_SYSTEM_ERROR(errno, path_);
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
		FLUX_SYSTEM_ERROR(errno, path_);
	}
	exists_ = true;
	return true;
}

} // namespace flux
