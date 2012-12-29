/*
 * Instance.hpp -- reference counting and secure destruction
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_INSTANCE_HPP
#define FTL_INSTANCE_HPP

#include "defaults.hpp"
#include "Exception.hpp"
#include "SpinLock.hpp"

namespace ftl
{

FTL_EXCEPTION(ReferenceException, Exception);

class BackRef
{
public:
	BackRef(void** instance): instance_(instance) {}
	BackRef *pred_;
	BackRef *succ_;
	void** instance_;
	#ifdef FTL_REF_THREADSAFE_SET
	SpinLock mutex_;
	#endif
};

/** \brief reference counting and secure destruction
  *
  * Base class for all classes T, whose instances can be referred to by Ref<T>.
  * Enforces a consistent allocation schema by surpressing two things:
  *   - combination of static allocation and dynamic destruction
  *   - manual detruction by delete operator
  * In both cases an exception of type ReferenceException is thrown.
  */
class RefCounter: public SpinLock
{
public:
	RefCounter(): refCount_(0) {}

	virtual ~RefCounter()
	{
#ifndef NDEBUG
		if (refCount_ > 0)
			FTL_THROW(ReferenceException, "Deleting object, which is still in use");
#endif
	}

	inline int refCount() const { return refCount_; }

	inline void incRefCount() {
		__sync_add_and_fetch(&refCount_, 1);
	}

	inline void decRefCount() {
		if (__sync_sub_and_fetch(&refCount_, 1) == 0)
			delete this;
	}

private:
	RefCounter(const RefCounter &);
	const RefCounter &operator=(const RefCounter &);

	volatile int refCount_;
};

class BackRefList
{
public:
	BackRefList(): backRefHead_(0) {}

	virtual ~BackRefList()
	{
		if (backRefHead_) {
			mutex_.acquire();
			BackRef *ref = backRefHead_;
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

	inline void addBackRef(BackRef *ref)
	{
		mutex_.acquire();
		ref->succ_ = backRefHead_;
		ref->pred_ = 0;
		if (backRefHead_)
			backRefHead_->pred_ = ref;
		backRefHead_ = ref;
		mutex_.release();
	}

	inline void delBackRef(BackRef *ref)
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
	BackRefList(const BackRefList &b): backRefHead_(0) {}
	inline const BackRefList &operator=(const BackRefList &b) { return *this; }

private:
	SpinLock mutex_;
	BackRef *backRefHead_;
};

template<class T> class FTL_DEFAULT_REF_POLICY;
template<class T, template<class> class P> class Ref;

class Instance: public RefCounter
#ifndef NDEBUG
, public virtual BackRefList
#endif
{};

} // namespace ftl

#endif // FTL_INSTANCE_HPP
