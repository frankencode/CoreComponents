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
#include <cc/LocalStatic>
#include <cc/ThreadLocalSingleton>

namespace cc {

template<class SubClass>
class Singleton;

template<class SubClass>
class CoreSingleton
{
public:
    static SubClass *instance()
    {
        SpinLock &mutex = localStatic<SpinLock, SubClass>();
        Guard<SpinLock> guard(&mutex);
        Ref<SubClass> &instance_ = localStatic< Ref<SubClass>, CoreSingleton<SubClass> >();
        if (!instance_)
            instance_ = Singleton<SubClass>::create();
        return instance_;
    }
};

template<class SubClass>
class CoreSingletonWrapper: public Object
{
public:
    CoreSingletonWrapper():
        instance_(CoreSingleton<SubClass>::instance())
    {}
    Ref<SubClass> instance_;
};

/** \class Singleton Singleton.h cc/Singleton
  * \brief Thread-safe process-wide singletons
  *
  * The following shows how to implement a thread-safe singleton using the Singleton<T> template class.
  * Returning const pointers to the singleton instance isn't a must but a good policy in multi-threaded applications.
  *
  * %Singleton declaration (example):
  * ~~~~~~~~~~~~~
  * class Settings: public Object
  * {
  * public:
  *     static const Settings *instance();
  * private:
  *     friend class Singleton<Settings>;
  *     Settings();
  * };
  * ~~~~~~~~~~~~~
  *
  * %Singleton implementation (example):
  * ~~~~~~~~~~~~~
  * #include <cc/Singleton>
  * ...
  * const Settings *Settings::instance() { return Singleton<Settings>::instance() }
  * Settings::Settings() {}
  * ~~~~~~~~~~~~~
  */
template<class SubClass>
class Singleton: public ThreadLocalSingleton< CoreSingletonWrapper<SubClass> >
{
public:
    inline static SubClass *instance() {
        return ThreadLocalSingleton< CoreSingletonWrapper<SubClass> >::instance()->instance_;
    }
private:
    friend class CoreSingleton<SubClass>;
    inline static SubClass *create() { return new SubClass; }
};

} // namespace cc
