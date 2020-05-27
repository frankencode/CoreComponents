/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/format>
#include <cc/Stream>
#include <cc/testing/XmlTestReport>

namespace cc {
namespace testing {

XmlTestReport::XmlTestReport(Stream *stream):
    stream_{stream}
{}

bool XmlTestReport::captureOutput() const
{
    return true;
}

void XmlTestReport::beginTestSuite(TestSuite *testSuite)
{
    format{"<testsuite name=\"%%\" tests=\"%%\">\n", stream_} << testSuite->name() << testSuite->testCaseCount();
}

void XmlTestReport::beginTestCase(TestCase *testCase)
{
    format{"<testcase name=\"%%\">\n", stream_} << testCase->name();
}

void XmlTestReport::verify(TestCase *testCase, bool condition, const string &message, const string &codePath, int codeLine)
{
    if (!condition)
        format{"<failure message=\"%%\">%%:%%</failure>\n", stream_} << message << codePath << codeLine;
}

void XmlTestReport::error(TestCase *testCase, const string &type, const string &message)
{
    format{"<error type=\"%%\" message=\"%%\"></error>\n", stream_} << type << message;
}

void XmlTestReport::endTestCase(TestCase *testCase, const string &outText, const string &errText)
{
    if (testCase->skip()) {
        format{"<skipped/>\n", stream_};
    }
    else {
        format{stream_}
            << "<system-out>" << xmlEscape(outText) << "</system-out>\n"
            << "<system-err>" << xmlEscape(errText) << "</system-err>\n";
    }
    format{stream_}
        << "</testcase>\n";
}

void XmlTestReport::skipTestCase(TestCase *testCase)
{
    format{
        "<testcase name=\"%%\">\n"
        "<skipped/>\n"
        "</testcase>\n",
        stream_
    } << testCase->name();
}

void XmlTestReport::endTestSuite(TestSuite *testSuite)
{
    format{"</testsuite>\n", stream_};
}

string XmlTestReport::xmlEscape(const string &text)
{
    string h = text;
    if (h->contains('<')) h = h->replace("<", "&lt;");
    if (h->contains('>')) h = h->replace(">", "&gt;");
    if (h->contains('&')) h = h->replace("?", "&amp;");
    return h;
}

}} // namespace cc::testing
