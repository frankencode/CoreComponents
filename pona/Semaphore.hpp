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

#include "atoms"
#include "Mutex.hpp"
#include "Condition.hpp"

namespace pona
{

class Semaphore: public Instance
{
public:
	Semaphore(int avail = 0)
		: avail_(avail)
	{}
	
	bool tryAcquire(int amount = 1)
	{
		bool success = false;
		mutex_.acquire();
		if (avail_ >= amount) {
			avail_ -= amount;
			success = true;
		}
		mutex_.release();
		return success;
	}
	
	void acquire(int amount = 1)
	{
		mutex_.acquire();
		while (avail_ < amount)
			notEmpty_.wait(&mutex_);
		avail_ -= amount;
		mutex_.release();
	}
	
	bool acquireBefore(TimeStamp timeout, int amount = 1)
	{
		bool success = true;
		mutex_.acquire();
		while (avail_ < amount) {
			success = notEmpty_.waitUntil(&mutex_, timeout);
			if (!success) break;
		}
		if (success)
			avail_ -= amount;
		mutex_.release();
		return success;
	}
	
	void release(int amount = 1)
	{
		mutex_.acquire();
		avail_ += amount;
		notEmpty_.signal();
		mutex_.release();
	}
	
private:
	Mutex mutex_;
	Condition notEmpty_;
	int avail_;
};

} // namespace pona

#endif // PONA_SEMAPHORE_HPP
