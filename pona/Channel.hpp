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

#ifndef PONA_CHANNEL_HPP
#define PONA_CHANNEL_HPP

#include "Atoms.hpp"
#include "Queue.hpp"
#include "Monitor.hpp"

namespace pona
{

/** The Channel class provides a monitorable Queue.
  */
template<class T, int defaultCapa = 128>
class Channel: public MonitorMedia
{
public:
	Channel(int size = defaultCapa)
		: queue_(size),
		  monitor_(0)
	{}
	
	inline int size() const { return queue_.size(); }
	inline int fill() const { return queue_.fill(); }
	inline void setMonitor(Ref<Monitor> monitor) { monitor_ = monitor; }
	
	void pushBack(T e)
	{
		monitor_->beforeGrowing(-1, 1);
		queue_.pushBack(e);
		monitor_->afterGrowing(-1, 1);
	}
	
	void pushFront(T e)
	{
		monitor_->beforeGrowing(0, 1);
		queue_.pushFront(e);
		monitor_->afterGrowing(0, 1);
	}
	
	T popBack()
	{
		monitor_->beforeShrinking(-1, 1);
		T e = queue_.popBack();
		monitor_->afterShrinking(-1, 1);
		return e;
	}
	
	T popFront()
	{
		monitor_->beforeShrinking(0, 1);
		T e = queue_.popFront();
		monitor_->afterShrinking(0, 1);
		return e;
	}
	
	void clear()
	{
		monitor_->beforeShrinking(0, fill());
		queue_.clear();
		monitor_->afterShrinking(0, fill());
	}
	
	T back(int i)
	{
		monitor_->beforeReadonlyAccess(fill() - i, 1);
		T e = queue_.back(i);
		monitor_->afterReadonlyAccess(fill() - i, 1);
		return e;
	}
	
	T front(int i)
	{
		monitor_->beforeReadonlyAccess(i, 1);
		T e = queue_.front(i);
		monitor_->afterReadonlyAccess(i, 1);
		return e;
	}
	
private:
	Queue<T> queue_;
	Ref<Monitor, Owner> monitor_;
};

} // namespace pona

#endif // PONA_CHANNEL_HPP

