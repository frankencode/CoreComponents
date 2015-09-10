/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXTESTING_XMLTESTREPORT_H
#define FLUXTESTING_XMLTESTREPORT_H

#include <flux/testing/TestReport>

namespace flux {
namespace testing {

class XmlTestReportPrivate;

class XmlTestReport: public TestReport
{
public:
    inline static Ref<XmlTestReport> create(Stream *stream) { return new XmlTestReport(stream); }

    virtual void beginTestSuite(TestSuite *testSuite);
    virtual void beginTestCase(TestCase *testCase);
    virtual void verify(TestCase *testCase, bool condition, String message, String codePath, int codeLine);
    virtual void skip(TestCase *testCase);
    virtual void error(TestCase *testCase, String type, String message);
    virtual void endTestCase(TestCase *testCase, String outText = "", String errText = "");
    virtual void endTestSuite(TestSuite *testSuite);

protected:
    XmlTestReport(Stream *stream);

    Ref<Stream> stream_;
};

}} // namespace flux::testing

#endif // FLUXTESTING_XMLTESTREPORT_H
