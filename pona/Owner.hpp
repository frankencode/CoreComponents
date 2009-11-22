/*
 * Owner.hpp -- ownership 'Ref' policy
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_OWNER_HPP
#define PONA_OWNER_HPP

#include "defaults.hpp"
#ifdef PONA_REF_THREADSAFE_SET
#include "SpinGuard.hpp"
#endif

namespace pona
{

/** The Owner policy is threadsafe if the PONA_REF_THREADSAFE_SET compile flag is set.
  * In particular the following race conditions are handled:
  *   - concurrently calling set(T*) and ~T()
  *   - concurrently calling set((T*)0) and ~Ref<T,Owner>()
  *   - concurrently calling set(T*)
  */
template<class T>
class Owner
{
public:
	Owner(): instance_(0) {}
	
	inline void set(T* b)
	{
		#ifdef PONA_REF_THREADSAFE_SET
		SpinGuard guard(&spinMutex_);
		#endif
		
		if (instance_ != b)
		{
			if (instance_)
				instance_->decRefCount();
			
			instance_ = b;
			
			if (instance_)
				instance_->incRefCount();
		}
	}
	
	inline T* get() const { return instance_; }
	
private:
	T* instance_;
	#ifdef PONA_REF_THREADSAFE_SET
	SpinMutex spinMutex_;
	#endif
};

} // namespace pona

#endif // PONA_OWNER_HPP
