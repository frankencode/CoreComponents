/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/XmlTestReport>
#include <cc/Format>
#include <cc/Stream>

namespace cc {
namespace testing {

XmlTestReport::XmlTestReport(const Stream &stream):
    stream_{stream}
{}

bool XmlTestReport::captureOutput() const
{
    return true;
}

void XmlTestReport::beginTestSuite(TestSuite *testSuite)
{
    Format{"<testsuite name=\"%%\" tests=\"%%\">\n", stream_} << testSuite->name() << testSuite->testCaseCount();
}

void XmlTestReport::beginTestCase(TestCase *testCase)
{
    Format{"<testcase name=\"%%\">\n", stream_} << testCase->name();
}

void XmlTestReport::verify(TestCase *testCase, bool condition, const String &message, const String &codePath, int codeLine)
{
    if (!condition)
        Format{"<failure message=\"%%\">%%:%%</failure>\n", stream_} << message << codePath << codeLine;
}

void XmlTestReport::error(TestCase *testCase, const String &type, const String &message)
{
    Format{"<error type=\"%%\" message=\"%%\"></error>\n", stream_} << type << message;
}

void XmlTestReport::endTestCase(TestCase *testCase, const String &outText, const String &errText)
{
    if (testCase->skip()) {
        Format{"<skipped/>\n", stream_};
    }
    else {
        Format{stream_}
            << "<system-out>" << xmlEscape(outText) << "</system-out>\n"
            << "<system-err>" << xmlEscape(errText) << "</system-err>\n";
    }
    Format{stream_}
        << "</testcase>\n";
}

void XmlTestReport::skipTestCase(TestCase *testCase)
{
    Format{
        "<testcase name=\"%%\">\n"
        "<skipped/>\n"
        "</testcase>\n",
        stream_
    } << testCase->name();
}

void XmlTestReport::endTestSuite(TestSuite *testSuite)
{
    Format{"</testsuite>\n", stream_};
}

String XmlTestReport::xmlEscape(const String &text)
{
    String h = text;
    if (h->contains('<')) h = h->replace("<", "&lt;");
    if (h->contains('>')) h = h->replace(">", "&gt;");
    if (h->contains('&')) h = h->replace("?", "&amp;");
    return h;
}

}} // namespace cc::testing
