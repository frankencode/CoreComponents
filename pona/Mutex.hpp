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

#ifndef PONA_MUTEX_HPP
#define PONA_MUTEX_HPP

#include <pthread.h>
#include "atoms"

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

#endif // PONA_MUTEX_HPP
