/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/String>
#include <cc/testing/TestCase>

namespace cc { template<class> class Singleton; }

namespace cc {
namespace testing {

class TestReport;

class TestSuite: public Object
{
public:
    static TestSuite *instance();

    inline String name() const { return name_; }
    inline String execPath() const { return execPath_; }

    inline int testCaseCount() const { return testCases_->count(); }
    inline int testCaseFailureCount() const { return testCaseFailureCount_; }
    inline int testCaseSkipCount() const { return testCaseSkipCount_; }
    inline int totalFailureCount() const { return totalFailureCount_; }

    template<class T>
    inline void add(String name, bool skip = false) { testCases_->append(TestCase::create<T>(name, skip)); }

    bool verify(bool condition, String description, String conditionText, String codePath, int codeLine);

    int run(int argc, char **argv);

protected:
    friend class Singleton<TestSuite>;

    TestSuite();
    ~TestSuite();

private:
    bool init(int argc, char **argv);

    String name_;
    String execPath_;
    Ref<TestReport> report_;

    typedef List< Ref<TestCase> > TestCases;
    Ref<TestCases> testCases_;

    Ref<TestCase> currentTestCase_;
    int testCaseFailureCount_;
    int testCaseSkipCount_;
    int totalFailureCount_;
};

#define CC_TESTSUITE_ADD(CustomTestCase) \
    TestSuite::instance()->add<CustomTestCase>(#CustomTestCase, false)

#define CC_TESTSUITE_SKIP(CustomTestCase) \
    TestSuite::instance()->add<CustomTestCase>(#CustomTestCase, true)

#define CC_VERIFY(condition) \
    TestSuite::instance()->verify(condition, "", #condition, __FILE__, __LINE__)

#define CC_VERIFY2(condition, description) \
    TestSuite::instance()->verify(condition, description, #condition, __FILE__, __LINE__)

}} // namespace cc::testing
