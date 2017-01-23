/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/types>

namespace cc {

/** \class Ref Ref.h cc/Object
  * \ingroup mman
  * \brief %Object references
  *
  * The Ref template class implements object references that behave similiar to object
  * references in higher level languages, like for instance Java. The references can
  * be freely assigned and passed in places where a pointer to an object of compatible
  * type is expected. In constrast to simple pointers type conversions on assignment are
  * done automatically. Most of the time there shouldn't be a need to invoke C++'s lowlevel
  * type casting functions manually. The only exception is if a reference is passed to an
  * interface that would require an upcast in the type hierarchy. In this case you can
  * use the cast<T>() helper function, which safely selects the proper low-level type cast.
  * If the type compatiblity can only be resolved at compile time the reference will be
  * assigned a null pointer in the same way a dynamic_cast would results in a null pointer,
  * but knowledge of these low-level concepts is not required to use the Ref class properly.
  *   The type T needs to be inherited from the Object class, which provides the reference
  * count for an object. The reference count starts with zero when the object is created and
  * is incremented each time a new Ref<T> starts pointing to that object. In the same fasion
  * the object's reference count is decremented whenever a Ref<T> stops pointing to that object.
  * When the object's reference count reaches zero back again the object will be deleted
  * automatically. Both reference counting and automatic destruction is guaranteed to be thread-safe.
  *   Whenever an object is created an initial reference should be created with it.
  * Otherwise memory leaks might appear. Best practise is to use a named static constructor
  * method and hide the C++ "constructor" (in reality the initializer) as a protected or
  * private method. For instance see File::open(). It also results in more flexible and
  * more readable code.
  *   The Ref template class does purposely not implement a copy-on-write (COW) strategy.
  * This means objects, whose reference count is greater than 1, are not cloned automatically,
  * when accessed in a modifying way. Objects have to be cloned manually and you have to be
  * aware about the locality and thread-safety issues that arise as a consequence. For passing
  * objects amoung threads best practise is to use the Channel class and ensure that two threads
  * never hold a reference to the same object at the same time (shared nothing architecture).
  * If nevertheless objects need to be shared amoung threads at the same time (for instance a singleton)
  * make sure those objects are immutable and cannot be modified accidentally after construction.
  *   The only copy automatism the Ref template class implements is preemptive copy on assignment of a
  * const pointer (const T *). In this case a T::copy(const T *) is called to create a copy of the
  * of the object before assignment. This copying is needed because each Ref<T> allows modification
  * of the referenced object.
  *
  * \see Singleton
  */
template<class T>
class Ref
{
public:
    Ref(): a_(0) {}
    Ref(Ref &&b): a_(b.a_) { b.a_ = 0; }
    Ref(const Ref &b): a_(0) { set(b.a_); }
    template<class T2> Ref(const Ref<T2> &b): a_(0) { set(cast<T>(b.get())); }

    Ref(T *b): a_(0) { set(b); }
    template<class T2> Ref(T2 *b): a_(0) { set(cast<T>(b)); }
    template<class T2> Ref(const T2 *b): a_(0) { set(T::copy(cast<const T>(b))); }

    ~Ref() { set(0); }

    inline const Ref &operator=(Ref &&b) { set(0); a_ = b.a_; b.a_ = 0; return *this; }
    inline const Ref &operator=(const Ref &b) { set(b.a_); return *this; }
    inline const Ref &operator=(T *b) { set(b); return *this; }
    inline operator T *() const { return a_; }

    template<class T2> inline const Ref &operator=(T2 *b) { set(cast<T>(b)); return *this; }
    template<class T2> inline const Ref &operator=(const T2 *b) { set(T::copy(cast<const T>(b))); return *this; }
    template<class T2> inline const Ref &operator=(const Ref<T2> &b) { set(cast<T>(b.get())); return *this; }

    inline bool operator<(const Ref &b) const { return a_ < b.a_; }

    inline T &operator*() {
        CC_ASSERT2(a_, "Null reference accessed");
        return *a_;
    }

    inline T *operator->() {
        CC_ASSERT2(a_, "Null reference accessed");
        return a_;
    }

    inline const T *operator->() const {
        CC_ASSERT2(a_, "Null reference accessed");
        return a_;
    }

    inline T *get() const { return a_; }

    inline void set(T *b) {
        if (a_ != b) {
            if (b) b->incRefCount();
            if (a_) a_->decRefCount();
            a_ = b;
        }
    }

    /// helper method to support implicit deep copies of object lists
    inline static Ref copy(const Ref &b) { return T::copy(b.a_); }

    template<class T2>
    inline Ref<T> operator<<(T2 x) {
        CC_ASSERT2(a_, "Null reference on shift left");
        *a_ << x;
        return *this;
    }

    template<class T2>
    inline Ref<T> operator>>(T2 &x) {
        CC_ASSERT2(a_, "Null reference on shift right");
        *a_ >> x;
        return *this;
    }

private:
    template<class U, class V>
    inline static U *cast(V *p) { return CastHelper<V, U, ConversionFromTo<V*, U*>::Exists>::cast(p); }

    T *a_;
};

} // namespace cc
