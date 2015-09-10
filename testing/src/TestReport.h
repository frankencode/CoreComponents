/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXTESTING_TESTREPORT_H
#define FLUXTESTING_TESTREPORT_H

#include <flux/testing/TestSuite>
#include <flux/testing/TestCase>

namespace flux {
namespace testing {

class TestReport: public Object
{
public:
    virtual void beginTestSuite(TestSuite *testSuite) = 0;
    virtual void beginTestCase(TestCase *testCase) = 0;
    virtual void verify(TestCase *testCase, bool condition, String message, String codePath, int codeLine) = 0;
    virtual void skip(TestCase *testCase) = 0;
    virtual void error(TestCase *testCase, String type, String message) = 0;
    virtual void endTestCase(TestCase *testCase, String outText = "", String errText = "") = 0;
    virtual void endTestSuite(TestSuite *testSuite) = 0;
};

}} // namespace flux::testing

#endif // FLUXTESTING_TESTREPORT_H
