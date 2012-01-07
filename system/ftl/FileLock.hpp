/*
 * FileLock.hpp -- advisory file locks
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
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

class FileLock: public FLockStruct, public Instance
{
public:
	enum Type { Read = F_RDLCK, Write = F_WRLCK };
	
	FileLock(Ref<File> file, int type, off_t start = 0, off_t length = 0);
	
	bool tryAcquire();
	void acquire();
	void release();
	
private:
	int fd_;
};

} // namespace ftl

#endif // FTL_FILELOCK_HPP
