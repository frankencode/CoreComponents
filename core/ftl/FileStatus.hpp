 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_FILESTATUS_HPP
#define FTL_FILESTATUS_HPP

#include <sys/types.h>
#include <sys/stat.h>
#include "Time.hpp"
#include "string.hpp"
#include "SystemStream.hpp"

namespace ftl
{

class File;

typedef struct stat StructStat;

class FileStatus: public StructStat, public Instance
{
public:
	inline static Ref<FileStatus> read(int fd = -1) { return new FileStatus(fd); }
	inline static Ref<FileStatus> read(SystemStream *stream) { return read(stream->fd()); }
	inline static Ref<FileStatus> read(string path, bool resolve = true) { return new FileStatus(path, resolve); }

	inline string path() const { return path_; }

	inline int type() const  { return st_mode & S_IFMT; }
	inline int mode() const { return st_mode & (~S_IFMT); }

	inline off_t size() const { return st_size; }
	inline off_t sizeOfBlock() const { return st_blksize; }
	inline off_t sizeInBlocks() const { return st_blocks; }

	inline uid_t ownerId() const { return st_uid; }
	inline gid_t groupId() const { return st_gid; }

	inline Time lastAccess() const { return Time(st_atime, 0); }
	inline Time lastModified() const { return Time(st_mtime, 0); }
	inline Time lastChanged() const { return Time(st_ctime, 0); }
	void setTimes(Time lastAccess, Time lastModified);

	inline ino_t inodeNumber() const { return st_ino; }
	inline nlink_t numberOfHardLinks() const { return st_nlink; }

	inline dev_t storageId() const { return st_dev; }
	inline dev_t deviceId() const { return st_rdev; }

	inline bool exists() const { return exists_; }

private:
	FileStatus(int fd);
	FileStatus(string path, bool resolve = true);

	bool update();

	int fd_;
	string path_;
	bool exists_;
	bool resolve_;
};

} // namespace ftl

#endif // FTL_FILESTATUS_HPP
