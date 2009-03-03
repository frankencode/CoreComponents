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

#ifndef PONA_BOUNCER_HPP
#define PONA_BOUNCER_HPP

#include "Monitor.hpp"
#include "Mutex.hpp"
#include "Condition.hpp"

namespace pona
{

/** \brief Synchronising concurrent media access
  *
  * A bouncer synchronizes concurrent access of different threads to a media.
  * It prevents media under- and overflows (0 <= media->fill() <= media->size())
  * and overlapping access operations (fully exclusive).
  *
  * If the media is full (media->fill() == media->size()) threads which reduce
  * the number of items are scheduled in favour to threads which produce items.
  * The media may be resized at any time without affected the synchronization logic.
  */
class Bouncer: public Monitor
{
public:
	Bouncer(Ref<MonitorMedia> media);
	
private:
	virtual void beforeGrowing(int i, int n);
	virtual void afterGrowing(int i, int n);
	
	virtual void beforeShrinking(int i, int n);
	virtual void afterShrinking(int i, int n);
	
	virtual void beforeAccess(int i, int n);
	virtual void afterAccess(int i, int n);
	
	virtual void beforeReadonlyAccess(int i, int n);
	virtual void afterReadonlyAccess(int i, int n);
	
	Ref<Condition> itemsAdded();
	Ref<Condition> itemsRemoved();
	
	Ref<MonitorMedia, SetNull> media_;
	Ref<Mutex, Owner> mutex_;
	Ref<Condition, Owner> itemsAdded_;
	Ref<Condition, Owner> itemsRemoved_;
};

} // namespace pona

#endif // PONA_BOUNCER_HPP
