/*
 * Instance.hpp -- reference counting and secure destruction
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_INSTANCE_HPP
#define FTL_INSTANCE_HPP

#include "defaults.hpp"
#include "Exception.hpp"
#ifndef NDEBUG
#include "CoreMutex.hpp"
#endif

namespace ftl
{

#ifndef NDEBUG
class BackRef
{
public:
	BackRef(void** instance): instance_(instance) {}
	BackRef* pred_;
	BackRef* succ_;
	void** instance_;
	#ifdef FTL_REF_THREADSAFE_SET
	CoreMutex mutex_;
	#endif
};
#endif // ndef NDEBUG

/** \brief reference counting and secure destruction
  *
  * Base class for all classes T, which instances can be referred to by Ref<T>.
  * Enforces a consistent allocation schema by surpressing two things:
  *   - combination of static allocation and dynamic destruction
  *   - manual detruction by delete operator
  * In both cases an exception of type ReferenceException is thrown.
  */
class Instance
{
public:
	Instance()
		: refCount_(0)
#ifndef NDEBUG
		  , backRefHead_(0)
#endif
	{}
	
	virtual ~Instance()
	{
#ifndef NDEBUG
		if (refCount_ > 0)
			FTL_THROW(DebugException, "Deleting object, which is still in use");
		if (backRefHead_) {
			mutex_.acquire();
			BackRef* ref = backRefHead_;
			while (ref) {
				#ifdef FTL_REF_THREADSAFE_SET
				ref->mutex_.acquire();
				#endif
				*ref->instance_ = 0;
				#ifdef FTL_REF_THREADSAFE_SET
				ref->mutex_.release();
				#endif
				ref = ref->succ_;
			}
			backRefHead_ = 0;
			mutex_.release();
		}
#endif
	}
	
#ifndef NDEBUG
	inline void addBackRef(BackRef* ref)
	{
		mutex_.acquire();
		ref->succ_ = backRefHead_;
		ref->pred_ = 0;
		if (backRefHead_)
			backRefHead_->pred_ = ref;
		backRefHead_ = ref;
		mutex_.release();
	}
	
	inline void delBackRef(BackRef* ref)
	{
		mutex_.acquire();
		if (ref->pred_)
			ref->pred_->succ_ = ref->succ_;
		if (ref->succ_)
			ref->succ_->pred_ = ref->pred_;
		if (ref == backRefHead_)
			backRefHead_ = backRefHead_->succ_;
		mutex_.release();
	}
#endif // ndef NDEBUG
	
	inline int refCount() const { return refCount_; }
	
	inline void incRefCount()
	{
		__sync_add_and_fetch(&refCount_, 1);
	}
	
	inline void decRefCount()
	{
		if (__sync_sub_and_fetch(&refCount_, 1) == 0)
			delete this;
	}
	
	Instance(const Instance& b)
		: refCount_(0)
#ifndef NDEBUG
		  , backRefHead_(0)
#endif
	{}
	inline const Instance& operator=(const Instance& b) { return *this; }
	
private:
	int refCount_;
	
#ifndef NDEBUG
	CoreMutex mutex_;
	BackRef* backRefHead_;
#endif
};

} // namespace ftl

#endif // FTL_INSTANCE_HPP
