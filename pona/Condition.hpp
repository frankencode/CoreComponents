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

#ifndef PONA_CONDITION_HPP
#define PONA_CONDITION_HPP

#include <pthread.h>
#include "atoms"
#include "Mutex.hpp"

namespace pona
{

class Condition: public Instance
{
public:
	Condition()
	{
		int ret = pthread_cond_init(&cond_, 0);
		if (ret != 0)
			PONA_SYSTEM_EXCEPTION;
	}
	
	~Condition()
	{
		int ret = pthread_cond_destroy(&cond_);
		if (ret != 0)
			PONA_SYSTEM_EXCEPTION;
	}
	
	inline void wait(Mutex* mutex)
	{
		int ret = pthread_cond_wait(&cond_, &mutex->mutex_);
		if (ret != 0)
			PONA_SYSTEM_EXCEPTION;
	}
	
	inline void signal()
	{
		int ret = pthread_cond_signal(&cond_);
		if (ret != 0)
			PONA_SYSTEM_EXCEPTION;
	}
	
	inline void broadcast()
	{
		int ret = pthread_cond_broadcast(&cond_);
		if (ret != 0)
			PONA_SYSTEM_EXCEPTION;
	}
	
private:
	pthread_cond_t cond_;
};

} // namespace pona

#endif // PONA_CONDITION_HPP
