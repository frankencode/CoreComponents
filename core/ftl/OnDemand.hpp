 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_ONDEMAND_HPP
#define FTL_ONDEMAND_HPP

#include "Ref.hpp"

namespace ftl
{

template<class T>
class OnDemand
{
public:
	inline void set(T *b) {}

	inline T *get() const {
		if (!instance_)
			instance_ = T::create();
		return instance_;
	}

private:
	mutable Ref<T, Owner> instance_;
};

} // namespace ftl

#endif // FTL_ONDEMAND_HPP
