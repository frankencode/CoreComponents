/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXTESTING_TTYTESTREPORT_H
#define FLUXTESTING_TTYTESTREPORT_H

#include <flux/testing/TestReport>

namespace flux {
namespace testing {

class TtyTestReport: public TestReport
{
public:
    inline static Ref<TtyTestReport> create() { return new TtyTestReport; }

    virtual bool captureOutput() const;

    virtual void beginTestSuite(TestSuite *testSuite);
    virtual void beginTestCase(TestCase *testCase);
    virtual void verify(TestCase *testCase, bool condition, String message, String codePath, int codeLine);
    virtual void error(TestCase *testCase, String type, String message);
    virtual void endTestCase(TestCase *testCase, String outText = "", String errText = "");
    virtual void skipTestCase(TestCase *testCase);
    virtual void endTestSuite(TestSuite *testSuite);

protected:
    TtyTestReport();
};

}} // namespace flux::testing

#endif // FLUXTESTING_TTYTESTREPORT_H
