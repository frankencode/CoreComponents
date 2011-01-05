/*
 * Pointer.hpp -- pointer 'Ref' policy
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_POINTER_HPP
#define FTL_POINTER_HPP

namespace ftl
{

template<class T>
class Pointer
{
public:
	Pointer(): instance_(0) {}
	
	inline void set(T* b) { instance_ = b; }
	inline T* get() const { return instance_; }
	
private:
	T* instance_;
};

} // namespace ftl

#endif // FTL_POINTER_HPP
