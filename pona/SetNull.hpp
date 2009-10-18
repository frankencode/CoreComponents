/*
 * SetNull.hpp -- setting to null 'Ref' policy
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_SETNULL_HPP
#define PONA_SETNULL_HPP

namespace pona
{

template<class T>
class PONA_API SetNull: private BackRef
{
public:
	SetNull(): BackRef((void**)&instance_), instance_(0) {}
	
	inline void set(T* b)
	{
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
};

} // namespace pona

#endif // PONA_SETNULL_HPP
