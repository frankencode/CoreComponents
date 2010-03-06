/*
 * Instance.hpp -- reference counting and secure destruction
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_INSTANCE_HPP
#define PONA_INSTANCE_HPP

#include "defaults.hpp"
#include "Exception.hpp"
#include "SpinMutex.hpp"

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
	#ifdef PONA_REF_THREADSAFE_SET
	SpinMutex mutex_;
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
class Instance
{
public:
	Instance(): refCount_(0), backRefList_(0) {}
	
	virtual ~Instance()
	{
		if (refCount_ > 0) {
			PONA_THROW(ReferenceException, "Deleting object, which is still in use");
		}
		if (backRefList_) {
			mutex_.acquire();
			BackRef* ref = backRefList_;
			while (ref) {
				#ifdef PONA_REF_THREADSAFE_SET
				ref->mutex_.acquire();
				#endif
				*ref->instance_ = 0;
				#ifdef PONA_REF_THREADSAFE_SET
				ref->mutex_.release();
				#endif
				ref = ref->succ_;
			}
			backRefList_ = 0;
			mutex_.release();
		}
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

	inline void addBackRef(BackRef* ref)
	{
		mutex_.acquire();
		ref->succ_ = backRefList_;
		ref->pred_ = 0;
		if (backRefList_)
			backRefList_->pred_ = ref;
		backRefList_ = ref;
		mutex_.release();
	}
	
	inline void delBackRef(BackRef* ref)
	{
		mutex_.acquire();
		if (ref->pred_)
			ref->pred_->succ_ = ref->succ_;
		if (ref->succ_)
			ref->succ_->pred_ = ref->pred_;
		if (ref == backRefList_)
			backRefList_ = backRefList_->succ_;
		mutex_.release();
	}
	
	inline SpinMutex* const mutex() { return &mutex_; }
	
	Instance(const Instance& b) {}
	inline const Instance& operator=(const Instance& b) { return *this; }
	
private:
	SpinMutex mutex_;
	int refCount_;
	BackRef* backRefList_;
};

} // namespace pona

#endif // PONA_INSTANCE_HPP
