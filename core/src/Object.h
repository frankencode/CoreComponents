/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Ref>

namespace cc {

/** \class Object Object.h cc/Object
  * \brief Reference counting and secure destruction
  *
  * Base class for all classes T, whose instances can be referred to by Ref<T>.
  * Enforces a consistent allocation schema by surpressing two things:
  *   - combination of static allocation and dynamic destruction
  *   - manual detruction by delete operator
  * In debug mode in both cases a DebugException is thrown.
  */
class Object
{
public:
    Object(): refCount_(0) {}

    virtual ~Object() {
        CC_ASSERT2(refCount_ == 0, "Deleting object, which is still in use");
    }

    inline int refCount() const { return refCount_; }

    inline void incRefCount() const {
        __sync_add_and_fetch(&refCount_, 1);
    }

    inline void decRefCount() const {
        if (__sync_sub_and_fetch(&refCount_, 1) == 0)
            delete this;
    }

private:
    Object(const Object &);
    const Object &operator=(const Object &);

    mutable volatile int refCount_;
};

} // namespace cc
