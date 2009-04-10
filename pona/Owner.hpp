/*
 * Owner.hpp -- ownership 'Ref' policy
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
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
				static_cast<Instance*>(instance_)->decRefCount();
			
			instance_ = b;
			
			if (instance_)
				static_cast<Instance*>(instance_)->incRefCount();
		}
	}
	
	inline T* get() const { return instance_; }
	
private:
	T* instance_;
};

} // namespace pona

#endif // PONA_OWNER_HPP
