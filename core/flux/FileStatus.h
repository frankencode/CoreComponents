/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_FILESTATUS_H
#define FLUX_FILESTATUS_H

#include <sys/types.h>
#include <sys/stat.h>
#include "String.h"
#include "SystemStream.h"

namespace flux
{

class File;

typedef struct stat StructStat;

class FileStatus: public StructStat, public Object
{
public:
	inline static Ref<FileStatus> read(int fd = -1) { return new FileStatus(fd); }
	inline static Ref<FileStatus> read(SystemStream *stream) { return read(stream->fd()); }
	inline static Ref<FileStatus> read(String path, bool resolve = true) { return new FileStatus(path, resolve); }

	inline String path() const { return path_; }

	inline int type() const  { return st_mode & S_IFMT; }
	inline int mode() const { return st_mode & (~S_IFMT); }

	inline off_t size() const { return st_size; }
	inline off_t sizeOfBlock() const { return st_blksize; }
	inline off_t sizeInBlocks() const { return st_blocks; }

	inline uid_t ownerId() const { return st_uid; }
	inline gid_t groupId() const { return st_gid; }

	inline double lastAccess() const { return st_atime; }
	inline double lastModified() const { return st_mtime; }
	inline double lastChanged() const { return st_ctime; }
	void setTimes(double lastAccess, double lastModified);

	inline ino_t inodeNumber() const { return st_ino; }
	inline nlink_t numberOfHardLinks() const { return st_nlink; }

	inline dev_t storageId() const { return st_dev; }
	inline dev_t deviceId() const { return st_rdev; }

	inline bool exists() const { return exists_; }

private:
	FileStatus(int fd);
	FileStatus(String path, bool resolve = true);

	bool update();

	int fd_;
	String path_;
	bool exists_;
	bool resolve_;
};

} // namespace flux

#endif // FLUX_FILESTATUS_H
