/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Object>
#include <cc/Ref>

namespace cc {

/** \brief Reference manually allocated objects
  *
  * Wrapper class to allow taking reference to classes not inherited
  * from cc::Object. Beware that it is impossible to reference
  * the same alien object by two different alien wrappers.
  */
template<class T>
class Alien: public Object
{
public:
    typedef void (*DelFunc)(T *);

    Alien(T *guest, DelFunc delFunc = 0):
        guest_(guest),
        delFunc_(delFunc)
    {}

    ~Alien()
    {
        if (guest_ != 0)
        {
            if (delFunc_) delFunc_(guest_);
            else delete guest_;
            guest_ = 0;
        }
    }

    inline T *get() const { return guest_; }

private:
    mutable T *guest_;
    DelFunc delFunc_;
};

/** \brief Objects which can perform an action
  */
class Action: public virtual Object
{
public:
    virtual void run() = 0;
};

/** \brief Objects which can be cloned
  */
template<class T>
class Clonable: public T
{
public:
    virtual Ref<T> clone() const = 0;
};

/** \brief Generic data source
  */
template<class T>
class Source: public virtual Object
{
public:
    virtual bool read(T *item) = 0;
};

/** \brief Generic data sink
  */
template<class T>
class Sink: public virtual Object
{
public:
    virtual void write(T item) = 0;
};

#ifdef major
#undef major
#endif
#ifdef minor
#undef minor
#endif

/** \brief Integer range
  */
class Range: public Object
{
public:
    inline static Ref<Range> create(int i0 = 0, int i1 = -1) { return new Range(i0, i1); }

    inline int i0() const { return i0_; }
    inline int i1() const { return i1_; }

    inline void assign(int i0, int i1) {
        i0_ = i0;
        i1_ = i1;
    }

    inline bool valid() const { return i0_ <= i1_; }

protected:
    Range(int i0 = 0, int i1 = -1): i0_(i0), i1_(i1) {}

    int i0_, i1_;
};

/** \brief Key value pair
  * \see Map
  */
template<class Key, class Value>
class Pair
{
public:
    Pair()
    {}

    Pair(const Key &key)
        : key_(key), value_(Value())
    {}

    Pair(const Key &key, const Value &value)
        : key_(key), value_(value)
    {}

    inline bool operator<(const Pair &b) const { return key_ <  b.key_; }
    inline bool operator>(const Pair &b) const { return key_ >  b.key_; }
    inline bool operator==(const Pair &b) const { return key_ ==  b.key_; }
    inline bool operator!=(const Pair &b) const { return key_ !=  b.key_; }
    inline bool operator<=(const Pair &b) const { return key_ <=  b.key_; }
    inline bool operator>=(const Pair &b) const { return key_ >=  b.key_; }

    inline const Key &key() const { return key_; }
    inline void setKey(const Key &key) { key_ = key; }

    inline const Value &value() const { return value_; }
    inline Value &value() { return value_; }
    inline void setValue(const Value &value) { value_ = value; }

    inline Pair *operator->() { return this; }
    inline const Pair *operator->() const { return this; }

private:
    Key key_;
    Value value_;
};

template<class Key, class Value>
inline Pair<Key, Value> pair(const Key &key, const Value &value) { return Pair<Key, Value>(key, value); }

} // namespace cc

