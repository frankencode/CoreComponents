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

#ifndef PONA_OWNER_HPP
#define PONA_OWNER_HPP

namespace pona
{

template<class T>
class Owner
{
public:
	Owner(): instance_(0) {}
	
	inline void set(T* b)
	{
		if (instance_ != b)
		{
			if (instance_)
			{
				static_cast<Instance*>(instance_)->decRefCount();
				if (static_cast<Instance*>(instance_)->refCount() == 0)
					static_cast<Instance*>(instance_)->destroy();
			}
			
			instance_ = b;
			
			if (instance_)
				static_cast<Instance*>(instance_)->incRefCount();
		}
	}
	
	inline T* get() const { return instance_; }
	
protected:
	T* instance_;
};

} // namespace pona

#endif // PONA_OWNER_HPP
