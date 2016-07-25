/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/testing/TestSuite>

namespace cc {
namespace testing {

class TestReport: public Object
{
public:
    virtual bool captureOutput() const = 0;

    virtual void beginTestSuite(TestSuite *testSuite) = 0;
    virtual void beginTestCase(TestCase *testCase) = 0;
    virtual void verify(TestCase *testCase, bool condition, String message, String codePath, int codeLine) = 0;
    virtual void error(TestCase *testCase, String type, String message) = 0;
    virtual void endTestCase(TestCase *testCase, String outText = "", String errText = "") = 0;
    virtual void skipTestCase(TestCase *testCase) = 0;
    virtual void endTestSuite(TestSuite *testSuite) = 0;
};

}} // namespace cc::testing

