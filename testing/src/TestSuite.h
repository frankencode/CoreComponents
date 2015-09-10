/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXTESTING_TESTSUITE_H
#define FLUXTESTING_TESTSUITE_H

#include <flux/String>
#include <flux/testing/TestCase>

namespace flux { template<class> class Singleton; }

namespace flux {
namespace testing {

class TestReport;

class TestSuite: public Object
{
public:
    String name() const { return name_; }
    int testCaseCount() const { return testCases_->count(); }
    int testCaseFailureCount() const { return testCaseFailureCount_; }
    int totalFailureCount() const { return totalFailureCount_; }

    template<class T>
    void add(String name, bool skip = false) { testCases_->append(TestCase::create<T>(name, skip)); }

    bool verify(bool condition, String description, String conditionText, String codePath, int codeLine);

    int run(int argc, char **argv);

protected:
    friend class Singleton<TestSuite>;

    TestSuite();
    ~TestSuite();

private:
    String name_;
    Ref<TestReport> report_;

    typedef List< Ref<TestCase> > TestCases;
    Ref<TestCases> testCases_;

    Ref<TestCase> currentTestCase_;
    int testCaseFailureCount_;
    int totalFailureCount_;
};

#define FLUX_TESTSUITE_ADD(CustomTestCase) \
    testSuite()->add<CustomTestCase>(#CustomTestCase, false)

#define FLUX_TESTSUITE_SKIP(CustomTestCase) \
    testSuite()->add<CustomTestCase>(#CustomTestCase, true)

#define FLUX_VERIFY(condition) \
    testSuite()->verify(condition, "", #condition, __FILE__, __LINE__)

#define FLUX_VERIFY2(condition, description) \
    testSuite()->verify(condition, description, #condition, __FILE__, __LINE__)

TestSuite *testSuite();

}} // namespace flux::testing

#endif // FLUXTESTING_TESTSUITE_H
