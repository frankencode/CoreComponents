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

#ifndef PONA_INSTANCE_HPP
#define PONA_INSTANCE_HPP

#include <stdio.h> // DEBUG
#include "Exception.hpp"

namespace pona
{

PONA_EXCEPTION(ReferenceException, Exception);

class Instance;

class BackRef
{
public:
	BackRef(void** instance): instance_(instance) {}
	BackRef* pred_;
	BackRef* succ_;
	void** instance_;
};

/** \brief reference counting and secure destruction
  *
  * Base class for all classes T, which instances can be referred to by Ref<T>.
  * Enforces a consistent allocation schema by suppressing two things:
  *   - combination of static allocation and dynamic destruction
  *   - manual detruction by delete operator
  * In both cases an exception of type ReferenceException is thrown.
  */
class Instance
{
public:
	Instance(): liberated_(false), refCount_(0), backRefList_(0) {}
	
	virtual ~Instance()
	{
		if ((refCount_ > 0) && (!liberated_))
			PONA_THROW(ReferenceException, "Deleting object, which is still in use");
		BackRef* ref = backRefList_;
		while (ref)
		{
			*(ref->instance_) = 0;
			ref = ref->succ_;
		}
	}
	
	inline int refCount() const { return refCount_; }
	inline void incRefCount() { ++refCount_; }
	inline void decRefCount() { --refCount_; }
	inline void destroy() { if (!liberated_) delete this; }
	
	inline void liberate() { liberated_ = true; }

	inline void addBackRef(BackRef* ref)
	{
		ref->succ_ = backRefList_;
		ref->pred_ = 0;
		if (backRefList_)
			backRefList_->pred_ = ref;
		backRefList_ = ref;
	}
	
	inline void delBackRef(BackRef* ref)
	{
		if (ref->pred_)
			ref->pred_->succ_ = ref->succ_;
		if (ref->succ_)
			ref->succ_->pred_ = ref->pred_;
		if (ref == backRefList_)
			backRefList_ = backRefList_->succ_;
	}
	
private:
	bool liberated_;
	int refCount_;
	BackRef* backRefList_;
};

} // namespace pona

#endif // PONA_INSTANCE_HPP
