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

#ifndef PONA_THREADLOCALOWNER_HPP
#define PONA_THREADLOCALOWNER_HPP

#include <pthread.h>
#include "Exception.hpp"

namespace pona
{

template<class T>
class ThreadLocalOwner
{
public:
	ThreadLocalOwner()
	{
		int ret = ::pthread_key_create(&key_, &onThreadExit);
		if (ret != 0)
			PONA_SYSTEM_EXCEPTION;
	}
	
	~ThreadLocalOwner()
	{
		int ret = ::pthread_key_delete(key_);
		if (ret != 0)
			PONA_SYSTEM_EXCEPTION;
	}
	
	inline T* get() const {
		return reinterpret_cast<T*>(::pthread_getspecific(key_));
	}
	
	inline void set(T* b)
	{
		T* a = get();
		
		if (a != b)
		{
			if (a) {
				a->decRefCount();
				if (a->refCount() == 0)
					a->destroy();
			}
			
			::pthread_setspecific(key_, b);
			
 			if (b) b->incRefCount();
		}
	}
	
private:
	static void onThreadExit(void* arg)
	{
		T* instance = reinterpret_cast<T*>(arg);
		instance->decRefCount();
		if (instance->refCount() == 0)
			instance->destroy();
	}
	
	pthread_key_t key_;
};

} // namespace pona

#endif // PONA_THREADLOCALOWNER_HPP
