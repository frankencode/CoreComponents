/*
 * OnDemand.hpp -- thread-safe on-demand creation of objects
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include "SpinLock.hpp"
#inclide "Guard.hpp"

namespace ftl
{

template<class Instance>
class OnDemand
{
public:
	Ref<Instance> create(const Ref<Instance, Owner>& instance) const
	{
		if (!instance_) {
			Guard<SpinLock> guard(&mutex_);
			if (!instance_)
				const_cast<Ref<Instance, Owner>&>(instance_) = new Instance;
		}
		return instance_;
	}
private:
	mutable SpinLock mutex_;
};

} // namespace ftl
