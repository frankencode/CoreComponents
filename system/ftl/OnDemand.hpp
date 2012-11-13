/*
 * OnDemand -- create objects on-demand
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
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
