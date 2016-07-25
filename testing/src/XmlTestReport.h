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

class XmlTestReport: public TestReport
{
public:
    inline static Ref<XmlTestReport> create(Stream *stream) { return new XmlTestReport(stream); }

    virtual bool captureOutput() const;

    virtual void beginTestSuite(TestSuite *testSuite);
    virtual void beginTestCase(TestCase *testCase);
    virtual void verify(TestCase *testCase, bool condition, String message, String codePath, int codeLine);
    virtual void error(TestCase *testCase, String type, String message);
    virtual void endTestCase(TestCase *testCase, String outText = "", String errText = "");
    virtual void skipTestCase(TestCase *testCase);
    virtual void endTestSuite(TestSuite *testSuite);

protected:
    XmlTestReport(Stream *stream);

private:
    static String xmlEscape(String text);

    Ref<Stream> stream_;
};

}} // namespace cc::testing
