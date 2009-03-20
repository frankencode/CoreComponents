/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstraction Library.
**
** Copyright (C) 2007-2009  Frank Mertens
**
** This file is part of a free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License as published
** by the Free Software Foundation, either version 3 of the License,
** or (at your option) any later version.
**
** The library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this libary.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef PONA_SHAREDMEMORY_HPP
#define PONA_SHAREDMEMORY_HPP

#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include "atoms"
#include "String.hpp"

namespace pona
{

class SharedMemory: public Instance
{
public:
	/** Create or attach to a shared memory.
	  * The shared memory is created, if it does not exist.
	  * The method firstInstance() returns true, if this object's
	  * construtor created the shared memory.
	  */
	SharedMemory(String name, int size)
		: name_(name),
		  nameUtf8_(name_.strdup()),
		  size_(size),
		  region_((uint8_t*)MAP_FAILED),
		  firstInstance_(true)
	{
		int fd = ::shm_open(nameUtf8_, O_CREAT|O_EXCL|O_RDWR, 0600);
		
		if (fd == -1)
		{
			if (errno != EEXIST)
				PONA_SYSTEM_EXCEPTION;
			
			fd = ::shm_open(nameUtf8_, O_RDWR, 0600);
			if (fd == -1)
				PONA_SYSTEM_EXCEPTION;
			
			firstInstance_ = false;
		}
		else
		{
			if (ftruncate(fd, size_) == -1)
				PONA_SYSTEM_EXCEPTION;
		}
		
		region_ = (uint8_t*)::mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
		if (region_ == (uint8_t*)MAP_FAILED)
			PONA_SYSTEM_EXCEPTION;
	}
	
	~SharedMemory()
	{
		if (region_ != (uint8_t*)MAP_FAILED) {
			if (::munmap(region_, size_) == -1)
				PONA_SYSTEM_EXCEPTION;
		}
		
		if (firstInstance_)
		{
			if (::shm_unlink(nameUtf8_) == -1)
				PONA_SYSTEM_EXCEPTION;
		}
		
		if (nameUtf8_) {
			::free(nameUtf8_);
			nameUtf8_ = 0;
		}
	}
	
	inline String name() const { return name_; }
	inline int size() const { return size_; }
	inline uint8_t* region() const { return region_; }
	inline bool firstInstance() const { return firstInstance_; }
	
private:
	String name_;
	char* nameUtf8_;
	int size_;
	uint8_t* region_;
	bool firstInstance_;
};

} // namespace pona

#endif // PONA_SHAREDMEMORY_HPP

