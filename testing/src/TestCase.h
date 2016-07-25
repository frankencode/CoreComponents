/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Object>
#include <cc/String>

namespace cc {
namespace testing {

class TestSuite;

class TestCase: public Object
{
public:
    inline String name() const { return name_; }
    inline bool skip() const { return skip_; }
    inline int assertionCount() const { return assertionCount_; }
    inline int failureCount() const { return failureCount_; }
    inline bool caughtException() const { return caughtException_; }
    inline bool passed() const { return failureCount_ == 0 && !caughtException_; }

protected:
    friend class TestSuite;

    template<class T>
    static Ref<TestCase> create(String name, bool skip)
    {
        Ref<T> t = new T;
        t->name_ = name;
        t->skip_ = skip;
        return t;
    }

    TestCase():
        skip_(false),
        assertionCount_(0),
        failureCount_(0),
        caughtException_(false)
    {}

    virtual void run() = 0;

private:
    String name_;
    bool skip_;
    int assertionCount_;
    int failureCount_;
    bool caughtException_;
};

}} // namespace cc::testing

