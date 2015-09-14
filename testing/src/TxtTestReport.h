/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXTESTING_TXTTESTREPORT_H
#define FLUXTESTING_TXTTESTREPORT_H

#include <flux/testing/TestReport>

namespace flux {
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

}} // namespace flux::testing

#endif // FLUXTESTING_TXTTESTREPORT_H
