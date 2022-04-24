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

struct TestSuite::State final: public Object::State
{
    State() = default;

    void init(int argc, char *argv[])
    {
        execPath = argv[0];
        name = execPath.baseName();

        Map<String, Variant> options {
            {"report", "txt"}
        };

        try {
            Arguments{argc, argv}.read(&options);

            String reportType = options.value("report");
            if (reportType == "txt")
                report = TxtTestReport{};
            else if (reportType == "xml")
                report = XmlTestReport{};
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
            ) <<  name;

            std::exit(1);
        }
    }

    bool verify(bool condition, const String &conditionText, const String &codePath, int codeLine)
    {
        if (!condition) {
            ++failureCount;
            ++currentTestCase->failureCount;
        }
        report.verify(currentTestCase, condition, conditionText, codePath, codeLine);
        return condition;
    }

    int run()
    {
        bool interactive = IoStream::output().isInteractive();

        report.beginTestSuite(TestSuite{});

        for (TestCase &testCase: testCases)
        {
            if (testCase.skip()) {
                report.skipTestCase(testCase);
                ++skipCount;
                continue;
            }

            report.beginTestCase(testCase);

            IoStream outSaved, errSaved;
            File outFile, errFile;
            if (report.captureOutput()) {
                outSaved = IoStream::output().duplicate();
                errSaved = IoStream::error().duplicate();
                outFile = TempFile{};
                errFile = TempFile{};
                outFile.duplicateTo(IoStream::output());
                errFile.duplicateTo(IoStream::error());
            }

            if (interactive) {
                currentTestCase = testCase;
                testCase.run();
            }
            else {
                try {
                    currentTestCase = testCase;
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
                    testCase->caughtException = true;
                    report.error(testCase, typeName, ex.message());
                }
                catch (...) {
                    testCase->caughtException = true;
                    report.error(testCase, "", "");
                }
            }

            String outText, errText;
            if (report.captureOutput()) {
                outSaved.duplicateTo(IoStream::output());
                errSaved.duplicateTo(IoStream::error());
                outText = outFile.map();
                errText = errFile.map();
            }

            report.endTestCase(testCase, outText, errText);

            if (testCase.failureCount() > 0 || testCase.caughtException()) ++failureCount;
        }

        report.endTestSuite(TestSuite{});

        return failureCount;
    }

    String execPath;
    String name;
    TestReport report;
    List<TestCase> testCases;
    TestCase currentTestCase;
    int failureCount { 0 };
    int skipCount { 0 };
    int totalFailureCount { 0 };
};

TestSuite::TestSuite()
{
    initOnce<State>();
}

TestSuite::TestSuite(int argc, char *argv[])
{
    initOnce<State>();
    me().init(argc, argv);
}

String TestSuite::execPath() const
{
    return me().execPath;
}

String TestSuite::name() const
{
    return me().name;
}

long TestSuite::testCaseCount() const
{
    return me().testCases.count();
}

bool TestSuite::verify(bool condition, const String &conditionText, const String &codePath, int codeLine)
{
    return me().verify(condition, conditionText, codePath, codeLine);
}

int TestSuite::run()
{
    return me().run();
}

void TestSuite::appendTestCase(const TestCase &testCase)
{
    me().testCases.append(testCase);
}

TestSuite::State &TestSuite::me()
{
    return Object::me.as<State>();
}

const TestSuite::State &TestSuite::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
