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

#ifndef PONA_SEMAPHORE_HPP
#define PONA_SEMAPHORE_HPP

#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>

#include "atoms"
#include "String.hpp"

#ifdef __MACH__
#include "Random.hpp"
#endif

namespace pona
{

class Semaphore: public Instance
{
public:
	Semaphore(int value = 0)
		: sem_(&semObj_),
		  firstInstance_(true)
	{
#ifdef __MACH__
		static Random semRandom_;
		
		while (true)
		{
			char name[] = { 's', 'e', 'm', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 0 };
			int key = semRandom_.next();
			for (int i = 0; i < 10; ++i, key /= 10)
				name[3+i] = '0' + (key % 10);
			sem_ = ::sem_open(name, O_CREAT|O_EXCL, 0600, unsigned(value));
			if (sem_ == (sem_t*)SEM_FAILED)
			{
				if (errno == EEXIST)
					continue;
				PONA_SYSTEM_EXCEPTION;
			}
			
			break;
		}
#else
		if (::sem_init(sem_, 0, unsigned(value)) == -1)
			PONA_SYSTEM_EXCEPTION;
#endif
	}
	
	Semaphore(String name, int value = 0)
		: firstInstance_(true)
	{
		char* nameUtf8 = name.strdup();
		
		sem_ = ::sem_open(nameUtf8, O_CREAT|O_EXCL, 0600, unsigned(value));
		
		if (sem_ == (sem_t*)SEM_FAILED)
		{
			if (errno != EEXIST)
				PONA_SYSTEM_EXCEPTION;
			
			sem_ = ::sem_open(nameUtf8, 0);
			if (sem_ == (sem_t*)SEM_FAILED)
				PONA_SYSTEM_EXCEPTION;
			
			firstInstance_ = false;
		}
		
		::free(nameUtf8);
	}
	
	~Semaphore()
	{
		if (sem_ == &semObj_)
		{
			if (::sem_destroy(sem_) == -1)
				PONA_SYSTEM_EXCEPTION;
		}
		else if (sem_ != (sem_t*)SEM_FAILED) 
		{
			if (::sem_close(sem_) == -1)
				PONA_SYSTEM_EXCEPTION;
		}
	}
	
	inline bool firstInstance() const { return firstInstance_; }
	
	inline void release()
	{
		if (::sem_post(sem_) == -1)
			PONA_SYSTEM_EXCEPTION;
	}
	
	inline void acquire()
	{
		if (::sem_wait(sem_) == -1)
			PONA_SYSTEM_EXCEPTION;
	}
	
	inline bool tryAcquire()
	{
		return ::sem_trywait(sem_) == 0;
	}
	
private:
	sem_t semObj_;
	sem_t* sem_;
	bool firstInstance_;
};

} // namespace pona

#endif // PONA_SEMAPHORE_HPP
