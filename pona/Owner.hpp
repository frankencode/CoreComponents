/*
 * Owner.hpp -- shared single-thread ownership 'Ref' policy
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_OWNER_HPP
#define PONA_OWNER_HPP

#include "defaults.hpp"
#ifdef PONA_REF_POLICY_RACE_DETECTION
#include <pthread.h>
#endif

namespace pona
{

template<class T>
class Owner
{
public:
	Owner()
		: instance_(0)
	#ifdef PONA_REF_POLICY_RACE_DETECTION
		  , creator_(pthread_self())
	#endif
	{}
	
	inline void set(T* b)
	{
		#ifdef PONA_REF_POLICY_RACE_DETECTION
		if (creator_ != pthread_self())
			PONA_THROW(RefException, "Illegal concurrent assignment");
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
	#ifdef PONA_REF_POLICY_RACE_DETECTION
	pthread_t creator_;
	#endif
};

} // namespace pona

#endif // PONA_OWNER_HPP
