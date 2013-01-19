 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_OWNER_HPP
#define FTL_OWNER_HPP

#include "defaults.hpp"
#ifdef FTL_REF_POLICY_RACE_DETECTION
#include <pthread.h>
#endif

namespace ftl
{

template<class T>
class Owner
{
public:
	Owner()
		: instance_(0)
	#ifdef FTL_REF_POLICY_RACE_DETECTION
		  , creator_(pthread_self())
	#endif
	{}

	inline void set(T *b)
	{
		#ifdef FTL_REF_POLICY_RACE_DETECTION
		if (creator_ != pthread_self())
			FTL_THROW(RefException, "Illegal concurrent assignment");
		#endif

		if (instance_ != b)
		{
			if (b)
				b->incRefCount();

			if (instance_)
				instance_->decRefCount();

			instance_ = b;
		}
	}

	inline T *get() const { return instance_; }

private:
	T *instance_;
	#ifdef FTL_REF_POLICY_RACE_DETECTION
	pthread_t creator_;
	#endif
};

} // namespace ftl

#endif // FTL_OWNER_HPP
