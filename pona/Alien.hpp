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

#ifndef PONA_ALIEN_HPP
#define PONA_ALIEN_HPP

#include "Instance.hpp"

namespace pona
{

/** Wrapper class to allow taking reference to classes not inherited
  * from pona::Instance. Please note, that it is impossible to reference
  * the same alien object by two different alien wrappers.
  */
template<class T>
class Alien: public Instance
{
public:
	typedef void (*DelFunc)(T*);

	Alien(T* external, DelFunc delFunc = 0)
		: external_(external),
		  delFunc_(delFunc)
	{}
	
	~Alien()
	{
		if (external_ != 0)
		{
			if (delFunc_) delFunc_(external_);
			else delete external_;
			external_ = 0;
		}
	}
	
	inline T* get() const { return external_; }
	
private:
	mutable T* external_;
	DelFunc delFunc_;
};


} // namespace pona

#endif // PONA_ALIEN_HPP
