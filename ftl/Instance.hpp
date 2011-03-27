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
#include "CoreMutex.hpp"

namespace ftl
{

FTL_EXCEPTION(ReferenceException, Exception);

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

/** \brief reference counting and secure destruction
  *
  * Base class for all classes T, which instances can be referred to by Ref<T>.
  * Enforces a consistent allocation schema by surpressing two things:
  *   - combination of static allocation and dynamic destruction
  *   - manual detruction by delete operator
  * In both cases an exception of type ReferenceException is thrown.
  */
class RefCounter
{
public:
	RefCounter(): refCount_(0) {}
	
	virtual ~RefCounter()
	{
		if (refCount_ > 0)
			FTL_THROW(ReferenceException, "Deleting object, which is still in use");
	}
	
	inline int refCount() const { return refCount_; }
	
	inline void incRefCount()
	{
		mutex_.acquire();
		refCount_ += (refCount_ >= 0);
		mutex_.release();
	}
	
	inline void decRefCount()
	{
		mutex_.acquire();
		refCount_ -= (refCount_ >= 0);
		if (refCount_ == 0) {
			mutex_.release();
			delete this;
		}
		else
			mutex_.release();
	}
	
	inline void liberate() { refCount_ = -1; }

	inline CoreMutex* const mutex() { return &mutex_; }
	
	RefCounter(const RefCounter& b): refCount_(0) {}
	inline const RefCounter& operator=(const RefCounter& b) {}
	
private:
	CoreMutex mutex_;
	int refCount_;
};

class BackRefList
{
public:
	BackRefList(): backRefHead_(0) {}
	
	virtual ~BackRefList()
	{
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
	}
	
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
	
	// ensure back reference lists are kept consistent on copying
	// (in theory an automomatically synthesized assignment operator in inherited class will invoke this)
	BackRefList(const BackRefList& b): backRefHead_(0) {}
	inline const BackRefList& operator=(const BackRefList& b) { return *this; }
	
private:
	CoreMutex mutex_;
	BackRef* backRefHead_;
};

#ifdef NDEBUG
typedef RefCounter Instance;
#else
class Instance: public RefCounter, public virtual BackRefList
{};
#endif

} // namespace ftl

#endif // FTL_INSTANCE_HPP
