/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_FILELOCK_H
#define FKIT_FILELOCK_H

#include <unistd.h>
#include <fcntl.h>
#include "File.h"

namespace fkit
{

typedef struct flock FLockStruct;

/** \brief advisory file locks
  * \see Guard
  */
class FileLock: public FLockStruct, public Object
{
public:
	enum Type { Read = F_RDLCK, Write = F_WRLCK };

	static Ref<FileLock> create(File *file, int type, off_t start = 0, off_t length = 0) {
		return new FileLock(file, type, start, length);
	}

	bool tryAcquire();
	void acquire();
	void release();

private:
	FileLock(File *file, int type, off_t start = 0, off_t length = 0);

	int fd_;
};

} // namespace fkit

#endif // FKIT_FILELOCK_H
