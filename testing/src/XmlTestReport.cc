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

bool XmlTestReport::captureOutput() const
{
    return true;
}

void XmlTestReport::beginTestSuite(const TestSuite &testSuite)
{
    fout("<testsuite name=\"%%\" tests=\"%%\">\n") << testSuite.name() << testSuite.testCaseCount();
}

void XmlTestReport::beginTestCase(const TestCase &testCase)
{
    fout("<testcase name=\"%%\">\n") << testCase.name();
}

void XmlTestReport::verify(const TestCase &testCase, bool condition, const String &message, const String &codePath, int codeLine)
{
    if (!condition)
        fout("<failure message=\"%%\">%%:%%</failure>\n") << message << codePath << codeLine;
}

void XmlTestReport::error(const TestCase &testCase, const String &type, const String &message)
{
    fout("<error type=\"%%\" message=\"%%\"></error>\n") << type << message;
}

void XmlTestReport::endTestCase(const TestCase &testCase, const String &outText, const String &errText)
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

void XmlTestReport::skipTestCase(const TestCase &testCase)
{
    fout(
        "<testcase name=\"%%\">\n"
        "<skipped/>\n"
        "</testcase>\n"
    ) << testCase.name();
}

void XmlTestReport::endTestSuite(const TestSuite &testSuite)
{
    fout() << "</testsuite>\n";
}

} // namespace cc
