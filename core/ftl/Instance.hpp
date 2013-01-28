 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_INSTANCE_HPP
#define FTL_INSTANCE_HPP

#include "types.hpp"
#include "O.hpp"
#include "Exception.hpp"

namespace ftl
{

FTL_EXCEPTION(ReferenceException, Exception);

/** \brief reference counting and secure destruction
  *
  * Base class for all classes T, whose instances can be referred to by O<T>.
  * Enforces a consistent allocation schema by surpressing two things:
  *   - combination of static allocation and dynamic destruction
  *   - manual detruction by delete operator
  * In both cases an exception of type ReferenceException is thrown.
  */
class Instance
{
public:
	Instance(): refCount_(0) {}

	virtual ~Instance()
	{
#ifndef NDEBUG
		if (refCount_ > 0)
			FTL_THROW(ReferenceException, "Deleting object, which is still in use");
#endif
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
	Instance(const Instance &);
	const Instance &operator=(const Instance &);

	mutable volatile int refCount_;
};

} // namespace ftl

#endif // FTL_INSTANCE_HPP
