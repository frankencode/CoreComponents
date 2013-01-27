 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_FILELOCK_HPP
#define FTL_FILELOCK_HPP

#include <unistd.h>
#include <fcntl.h>
#include "atoms"
#include "File.hpp"

namespace ftl
{

typedef struct flock FLockStruct;

/** \brief advisory file locks
  * \see Guard
  */
class FileLock: public FLockStruct, public Instance
{
public:
	enum Type { Read = F_RDLCK, Write = F_WRLCK };

	static O<FileLock> create(File *file, int type, off_t start = 0, off_t length = 0) {
		return new FileLock(file, type, start, length);
	}

	bool tryAcquire();
	void acquire();
	void release();

private:
	FileLock(File *file, int type, off_t start = 0, off_t length = 0);

	int fd_;
};

} // namespace ftl

#endif // FTL_FILELOCK_HPP
