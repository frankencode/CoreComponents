/*
 * Instance.hpp -- reference counting and secure destruction
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_INSTANCE_HPP
#define PONA_INSTANCE_HPP

#include "Exception.hpp"

namespace pona
{

PONA_EXCEPTION(ReferenceException, Exception);

class Instance;

class BackRef
{
public:
	BackRef(void** instance): instance_(instance) {}
	BackRef* pred_;
	BackRef* succ_;
	void** instance_;
};

/** \brief reference counting and secure destruction
  *
  * Base class for all classes T, which instances can be referred to by Ref<T>.
  * Enforces a consistent allocation schema by suppressing two things:
  *   - combination of static allocation and dynamic destruction
  *   - manual detruction by delete operator
  * In both cases an exception of type ReferenceException is thrown.
  */
class Instance
{
public:
	Instance(): refCount_(0), backRefList_(0) {}
	
	virtual ~Instance()
	{
		if (refCount_ > 0)
			PONA_THROW(ReferenceException, "Deleting object, which is still in use");
		BackRef* ref = backRefList_;
		while (ref) {
			*ref->instance_ = 0;
			ref = ref->succ_;
		}
	}
	
	inline int refCount() const { return refCount_; }
	inline void incRefCount() { refCount_ += (refCount_ >= 0); }
	inline void decRefCount() {
		refCount_ -= (refCount_ >= 0);
		if (refCount_ == 0)
				delete this;
	}
	
	inline void liberate() { refCount_ = -1; }

	virtual void acquire() {}
	virtual void release() {}
	
	inline void addBackRef(BackRef* ref)
	{
		acquire();
		ref->succ_ = backRefList_;
		ref->pred_ = 0;
		if (backRefList_)
			backRefList_->pred_ = ref;
		backRefList_ = ref;
		release();
	}
	
	inline void delBackRef(BackRef* ref)
	{
		acquire();
		if (ref->pred_)
			ref->pred_->succ_ = ref->succ_;
		if (ref->succ_)
			ref->succ_->pred_ = ref->pred_;
		if (ref == backRefList_)
			backRefList_ = backRefList_->succ_;
		release();
	}
	
private:
	int refCount_;
	BackRef* backRefList_;
};

} // namespace pona

#endif // PONA_INSTANCE_HPP
