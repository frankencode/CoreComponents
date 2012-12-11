/*
 * Pointer.hpp -- pointer 'Ref' policy
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
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

	inline void set(T *b) { instance_ = b; }
	inline T *get() const { return instance_; }

private:
	T *instance_;
};

} // namespace ftl

#endif // FTL_POINTER_HPP
