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

#ifndef PONA_MONITORMEDIA_HPP
#define PONA_MONITORMEDIA_HPP

#include "atoms"

namespace pona
{

class Monitor;

class MonitorMedia: public virtual Instance
{
public:
	virtual ~MonitorMedia() {}
	virtual int size() const = 0;
	virtual int fill() const = 0;
	virtual void setMonitor(Ref<Monitor> monitor) = 0;
};

} // namespace pona

#endif // PONA_MONITORMEDIA_HPP

