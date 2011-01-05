/*
 * FileLock.hpp -- advisory file locks
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
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
