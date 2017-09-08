/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Format>
#include <cc/Stream>
#include <cc/testing/XmlTestReport>

namespace cc {
namespace testing {

XmlTestReport::XmlTestReport(Stream *stream):
    stream_(stream)
{}

bool XmlTestReport::captureOutput() const
{
    return true;
}

void XmlTestReport::beginTestSuite(TestSuite *testSuite)
{
    Format("<testsuite name=\"%%\" tests=\"%%\">\n", stream_) << testSuite->name() << testSuite->testCaseCount();
}

void XmlTestReport::beginTestCase(TestCase *testCase)
{
    Format("<testcase name=\"%%\">\n", stream_) << testCase->name();
}

void XmlTestReport::verify(TestCase *testCase, bool condition, String message, String codePath, int codeLine)
{
    if (!condition)
        Format("<failure message=\"%%\">%%:%%</failure>\n", stream_) << message << codePath << codeLine;
}

void XmlTestReport::error(TestCase *testCase, String type, String message)
{
    Format("<error type=\"%%\" message=\"%%\"></error>\n", stream_) << type << message;
}

void XmlTestReport::endTestCase(TestCase *testCase, String outText, String errText)
{
    if (testCase->skip()) {
        Format("<skipped/>\n", stream_);
    }
    else {
        Format(stream_)
            << "<system-out>" << xmlEscape(outText) << "</system-out>\n"
            << "<system-err>" << xmlEscape(errText) << "</system-err>\n";
    }
    Format(stream_)
        << "</testcase>\n";
}

void XmlTestReport::skipTestCase(TestCase *testCase)
{
    Format(
        "<testcase name=\"%%\">\n"
        "<skipped/>\n"
        "</testcase>\n",
        stream_
    ) << testCase->name();
}

void XmlTestReport::endTestSuite(TestSuite *testSuite)
{
    Format("</testsuite>\n", stream_);
}

String XmlTestReport::xmlEscape(String text)
{
    if (text->contains('<')) text = text->replace("<", "&lt;");
    if (text->contains('>')) text = text->replace(">", "&gt;");
    if (text->contains('&')) text = text->replace("?", "&amp;");
    return text;
}

}} // namespace cc::testing
