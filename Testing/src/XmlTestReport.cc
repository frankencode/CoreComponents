/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/XmlTestReport>
#include <cc/TestSuite>
#include <cc/stdio>
#include <cc/input>

namespace cc {

struct XmlTestReport::State final: public TestReport::State
{
    bool captureOutput() const override
    {
        return true;
    }

    void beginTestSuite(const TestSuite &testSuite) override
    {
        fout("<testsuite name=\"%%\" tests=\"%%\">\n") << testSuite.name() << testSuite.testCaseCount();
    }

    void beginTestCase(const TestCase &testCase) override
    {
        fout("<testcase name=\"%%\">\n") << testCase.name();
    }

    void verify(const TestCase &testCase, bool condition, const String &message, const String &codePath, int codeLine) override
    {
        if (!condition)
            fout("<failure message=\"%%\">%%:%%</failure>\n") << message << codePath << codeLine;
    }

    void error(const TestCase &testCase, const String &type, const String &message) override
    {
        fout("<error type=\"%%\" message=\"%%\"></error>\n") << type << message;
    }

    void endTestCase(const TestCase &testCase, const String &outText = "", const String &errText = "") override
    {
        if (testCase.skip()) {
            fout() << "<skipped/>\n";
        }
        else {
            fout()
                << "<system-out>" << xmlEscape(outText) << "</system-out>\n"
                << "<system-err>" << xmlEscape(errText) << "</system-err>\n";
        }
        fout() << "</testcase>\n";
    }

    void skipTestCase(const TestCase &testCase) override
    {
        fout(
            "<testcase name=\"%%\">\n"
            "<skipped/>\n"
            "</testcase>\n"
        ) << testCase.name();
    }

    void endTestSuite(const TestSuite &testSuite) override
    {
        fout() << "</testsuite>\n";
    }
};

XmlTestReport::XmlTestReport():
    TestReport{new State}
{}

} // namespace cc
