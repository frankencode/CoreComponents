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

#ifndef PONA_MONITOR_HPP
#define PONA_MONITOR_HPP

#include "atoms"
#include "MonitorMedia.hpp"

namespace pona
{

/** \brief Monitoring operations on a media
  *
  * A monitor allows to track data operations on a media.
  * Medias which support monitoring implement the 'MonitorMedia' interface.
  *
  * There media reports about operations before and after they are performed.
  * These operation's semantics are defined as follows.
  *
  * Growing and shrinking operations change the number of items stored in a media.
  * Access operations read or write items, while readonly access operations read
  * items only.
  *
  * A growing operation may provide initial item values in addition to adding items.
  * A media does not need to report those operations as a separate access operation.
  *
  * The monitor interface is designed to enable the following areas of application:
  *   - thread synchronisation
  *   - MVC pattern in GUI programming
  *   - security auditing
  *   - redo / undo history
  *   - debugging
  *
  * Function parameters:
  *   i ... start index
  *   n ... number of items
  */
class Monitor: public virtual Instance
{
public:
	virtual void beforeGrowing(int i, int n) {}
	virtual void afterGrowing(int i, int n) {}
	
	virtual void beforeShrinking(int i, int n) {}
	virtual void afterShrinking(int i, int n) {}
	
	virtual void beforeAccess(int i, int n) {}
	virtual void afterAccess(int i, int n) {}
	
	virtual void beforeReadonlyAccess(int i, int n) {}
	virtual void afterReadonlyAccess(int i, int n) {}
};

} // namespace pona

#endif // PONA_MONITOR_HPP
