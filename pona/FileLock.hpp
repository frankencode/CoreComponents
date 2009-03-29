#ifndef PONA_FILELOCK_HPP
#define PONA_FILELOCK_HPP

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "atoms"
#include "File.hpp"

namespace pona
{

typedef struct flock FLockStruct;

class FileLock: public FLockStruct, public Instance
{
public:
	enum Type { Read = F_RDLCK, Write = F_WRLCK };
	
	FileLock(Ref<File> file, int type, off_t start = 0, off_t length = 0)
		: fd_(file->fd())
	{
		::memset(static_cast<FLockStruct*>(this), 0, sizeof(FLockStruct));
		FLockStruct::l_type = type;
		FLockStruct::l_start = start;
		FLockStruct::l_len = length;
		FLockStruct::l_whence = SEEK_SET;
		if (!file->isOpen())
			PONA_THROW(StreamSemanticException, "File needs to be opened, before it can be locked.");
	}
	
	bool tryAcquire()
	{
		bool success = (::fcntl(fd_, F_SETLK, static_cast<FLockStruct*>(this)) != -1);
		if (!success)
			if ((errno != EACCES) && (errno != EAGAIN))
				PONA_SYSTEM_EXCEPTION;
		return success;
	}
	
	void acquire()
	{
		while (true) {
			if (::fcntl(fd_, F_SETLKW, static_cast<FLockStruct*>(this)) == -1) {
				if (errno != EINTR)
					PONA_SYSTEM_EXCEPTION;
			}
			else
				break;
		}
	}
	
	void release()
	{
		struct flock h = *static_cast<FLockStruct*>(this);
		h.l_type = F_UNLCK;
		if (::fcntl(fd_, F_SETLK, &h) == -1)
			PONA_SYSTEM_EXCEPTION;
	}
	
private:
	int fd_;
};

} // namespace pona

#endif // PONA_FILELOCK_HPP
