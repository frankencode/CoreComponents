/*
 * Pointer.hpp -- pointer 'Ref' policy
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_POINTER_HPP
#define PONA_POINTER_HPP

namespace pona
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

template<class T>
class PointerInstance: public Pointer<T>, public Instance
{};

} // namespace pona

#endif // PONA_POINTER_HPP
