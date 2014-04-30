/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_OBJECT_H
#define FLUX_OBJECT_H

#include "Ref.h"

namespace flux
{

/** \brief reference counting and secure destruction
  *
  * Base class for all classes T, whose instances can be referred to by Ref<T>.
  * Enforces a consistent allocation schema by surpressing two things:
  *   - combination of static allocation and dynamic destruction
  *   - manual detruction by delete operator
  * In debug mode in both cases a DebugException is thrown.
  */
class Object
{
public:
	Object(): refCount_(0) {}

	virtual ~Object() {
		FLUX_ASSERT2(refCount_ == 0, "Deleting object, which is still in use");
	}

	inline int refCount() const { return refCount_; }

	inline void incRefCount() const {
		__sync_add_and_fetch(&refCount_, 1);
	}

	inline void decRefCount() const {
		if (__sync_sub_and_fetch(&refCount_, 1) == 0)
			delete this;
	}

private:
	Object(const Object &);
	const Object &operator=(const Object &);

	mutable volatile int refCount_;
};

} // namespace flux

#endif // FLUX_OBJECT_H
