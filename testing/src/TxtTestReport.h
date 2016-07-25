/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/testing/TestReport>

namespace cc {
namespace testing {

class TxtTestReport: public TestReport
{
public:
    inline static Ref<TxtTestReport> create() { return new TxtTestReport; }

    virtual bool captureOutput() const;

    virtual void beginTestSuite(TestSuite *testSuite);
    virtual void beginTestCase(TestCase *testCase);
    virtual void verify(TestCase *testCase, bool condition, String message, String codePath, int codeLine);
    virtual void error(TestCase *testCase, String type, String message);
    virtual void endTestCase(TestCase *testCase, String outText = "", String errText = "");
    virtual void skipTestCase(TestCase *testCase);
    virtual void endTestSuite(TestSuite *testSuite);

protected:
    TxtTestReport();
};

}} // namespace cc::testing

