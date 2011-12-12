/*
 * Owner.hpp -- shared single-thread ownership 'Ref' policy
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
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
	
	inline void set(T* b)
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
	
	inline T* get() const { return instance_; }
	
private:
	T* instance_;
	#ifdef FTL_REF_POLICY_RACE_DETECTION
	pthread_t creator_;
	#endif
};

} // namespace ftl

#endif // FTL_OWNER_HPP
