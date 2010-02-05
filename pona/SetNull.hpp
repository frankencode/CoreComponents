/*
 * SetNull.hpp -- setting to null 'Ref' policy
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_SETNULL_HPP
#define PONA_SETNULL_HPP

#include "defaults.hpp"
#ifdef PONA_REF_POLICY_DETECT_RACE_DETECTION
#include <pthread.h>
#endif
#include "Instance.hpp"

namespace pona
{

template<class T>
class SetNull: private BackRef
{
public:
	SetNull()
		: BackRef((void**)&instance_),
		  instance_(0)
	#ifdef PONA_REF_POLICY_RACE_DETECTION
		  ,creator_(pthread_self())
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
			if (instance_)
				instance_->delBackRef(this);
			
			instance_ = b;
			
			if (instance_)
				instance_->addBackRef(this);
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

#endif // PONA_SETNULL_HPP
