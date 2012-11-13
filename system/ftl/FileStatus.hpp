/*
 * FileStatus.hpp -- file type, size, ownership, etc.
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_FILESTATUS_HPP
#define FTL_FILESTATUS_HPP

#include <sys/types.h>
#include <sys/stat.h>
#include "atoms"
#include "Time.hpp"
#include "String.hpp"
#include "SystemStream.hpp"

namespace ftl
{

class File;

typedef struct stat StructStat;

class FileStatus: public StructStat, public Instance
{
public:
	inline static Ref<FileStatus, Owner> create(int fd = -1) { return new FileStatus(fd); }
	inline static Ref<FileStatus, Owner> create(Ref<SystemStream> stream) { return new FileStatus(stream); }
	inline static Ref<FileStatus, Owner> create(String path, bool resolve = true) { return new FileStatus(path, resolve); }

	inline String path() const { return path_; }

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
	FileStatus(Ref<SystemStream> stream);
	FileStatus(String path, bool resolve = true);

	bool update();

	int fd_;
	String path_;
	bool exists_;
	bool resolve_;
};

} // namespace ftl

#endif // FTL_FILESTATUS_HPP
