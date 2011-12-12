/*
 * OnDemand -- create objects on-demand
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

namespace ftl
{

template<class T>
class OnDemand
{
public:
	OnDemand(): firstTime_(true) {}
	
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
