/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstractions Library.
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

#ifndef PONA_MUTEX_HPP
#define PONA_MUTEX_HPP

#ifdef PONA_POSIX

#include <pthread.h>
#include "Atoms.hpp"

namespace pona
{

class Condition;

/** Non-recursive mutex.
 */
class Mutex: public Instance
{
public:
	Mutex()
	{
		pthread_mutex_init(&mutex_, 0);
	}

	~Mutex()
	{
		pthread_mutex_destroy(&mutex_);
	}

	inline void acquire()
	{
		pthread_mutex_lock(&mutex_);
	}

	inline void release()
	{
		pthread_mutex_unlock(&mutex_);
	}

private:
	friend class Condition;
	
	pthread_mutex_t mutex_;
};

} // namespace pona

#else // PONA_WINDOWS

#include <Windows.h>
#include "Atoms.hpp"

namespace pona
{

class Condition;

/** Non-recursive mutex.
 */
class Mutex: public Instance
{
public:
	Mutex()
	: locked_(false)
	{
		mutex_ = CreateMutex(0, FALSE, 0);
		if (!mutex_)
			PONA_WINDOWS_EXCEPTION;
	}

	~Mutex()
	{
		if (mutex_)
		{
			CloseHandle(mutex_);
			mutex_ = 0;
		}
	}

	inline void acquire()
	{
		if (WaitForSingleObject(mutex_, INFINITE) == WAIT_FAILED)
			PONA_WINDOWS_EXCEPTION;
		if (locked_)
			while (1) { Sleep(1000); } // simulate deadlock
		locked_ = true;
	}

	inline void release()
	{
		locked_ = false;
		if (!ReleaseMutex(mutex_))
			PONA_WINDOWS_EXCEPTION;
	}

private:
	friend class Condition;

	HANDLE mutex_;
	bool locked_;
};

} // namespace pona

#endif // platform switch

#endif // PONA_MUTEX_HPP
