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

namespace ftl
{

template<class T>
class OnDemand
{
public:
	OnDemand(): firstTime_(true) {}
	~OnDemand() { delete instance_; }
	
	inline void set(T* b) {}
	
	inline T* get() const {
		if (__sync_bool_compare_and_swap(&firstTime_, true, false))
			instance_ = new T;
		return instance_;
	}
	
private:
	mutable volatile bool firstTime_;
	mutable T* instance_;
};

} // namespace ftl
