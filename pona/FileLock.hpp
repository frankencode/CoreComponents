/*
 * FileLock.hpp -- advisory file locks
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_FILELOCK_HPP
#define PONA_FILELOCK_HPP

#include <unistd.h>
#include <fcntl.h>
#include "atoms"
#include "File.hpp"

namespace pona
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

} // namespace pona

#endif // PONA_FILELOCK_HPP
