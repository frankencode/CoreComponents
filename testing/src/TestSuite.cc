/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/TestSuite>
#include <cc/TxtTestReport>
#include <cc/XmlTestReport>
#include <cc/Arguments>
#include <cc/TempFile>
#include <cc/stdio>
#include <typeinfo> // typeid
#include <cstdlib> // exit, free
#include <cxxabi.h> // __cxa_demangle

namespace cc {

Handle<TestSuite::State> TestSuite::me { new TestSuite::State };

TestSuite::TestSuite(int argc, char *argv[])
{
    me().execPath = argv[0];
    me().name = me().execPath.baseName();

    Map<String, Variant> options {
        {"report", "txt"}
    };

    try {
        Arguments{argc, argv}.read(&options);

        String reportType = options.value("report");
        if (reportType == "txt")
            me().report = new TxtTestReport;
        else if (reportType == "xml")
            me().report = new XmlTestReport;
        else {
            ferr("Unsupported report type \"%%\"\n\n") << reportType;
            throw HelpRequest{};
        }
    }
    catch (HelpRequest &) {
        fout(
            "Usage: %% [OPTION]...\n"
            "Run test program.\n"
            "\n"
            "Options:\n"
            "  -report=[txt|xml]  Report type: plain text(txt) or JUnit(xml)\n"
            "\n"
        ) <<  me().name;

        std::exit(1);
    }
}

bool TestSuite::verify(bool condition, const String &conditionText, const String &codePath, int codeLine)
{
    if (!condition) {
        ++me().failureCount;
        ++me().currentTestCase.me().failureCount;
    }
    me().report().verify(me().currentTestCase, condition, conditionText, codePath, codeLine);
    return condition;
}

int TestSuite::run()
{
    bool interactive = IoStream::output().isInteractive();

    me().report().beginTestSuite(*this);

    for (TestCase &testCase: me().testCases)
    {
        if (testCase.skip()) {
            me().report().skipTestCase(testCase);
            ++me().skipCount;
            continue;
        }

        me().report().beginTestCase(testCase);

        IoStream outSaved, errSaved;
        File outFile, errFile;
        if (me().report().captureOutput()) {
            outSaved = IoStream::output().duplicate();
            errSaved = IoStream::error().duplicate();
            outFile = TempFile{};
            errFile = TempFile{};
            outFile.duplicateTo(IoStream::output());
            errFile.duplicateTo(IoStream::error());
        }

        if (interactive) {
            me().currentTestCase = testCase;
            testCase.run();
        }
        else {
            try {
                me().currentTestCase = testCase;
                testCase.run();
            }
            catch (Exception &ex) {
                String typeName = typeid(ex).name();
                {
                    int status = 0;
                    char *buf = abi::__cxa_demangle(typeName, 0, 0, &status);
                    if (status == 0) typeName = buf;
                    std::free(buf);
                }
                testCase.me().caughtException = true;
                me().report().error(testCase, typeName, ex.message());
            }
            catch (...) {
                testCase.me().caughtException = true;
                me().report().error(testCase, "", "");
            }
        }

        String outText, errText;
        if (me().report().captureOutput()) {
            outSaved.duplicateTo(IoStream::output());
            errSaved.duplicateTo(IoStream::error());
            outText = outFile.map();
            errText = errFile.map();
        }

        me().report().endTestCase(testCase, outText, errText);

        if (testCase.failureCount() > 0 || testCase.caughtException()) ++me().failureCount;
    }

    me().report().endTestSuite(*this);

    return me().failureCount;
}

} // namespace cc
