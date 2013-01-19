 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
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
