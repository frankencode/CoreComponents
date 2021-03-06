/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <typeinfo>
#include <cxxabi.h> // __cxa_demangle
#include <stdlib.h> // free(3)
#include <cc/exceptions>
#include <cc/stdio>
#include <cc/Singleton>
#include <cc/File>
#include <cc/TempFile>
#include <cc/Arguments>
#include <cc/testing/XmlTestReport>
#include <cc/testing/TxtTestReport>
#include <cc/testing/TestCase>
#include <cc/testing/TestSuite>

namespace cc {
namespace testing {

TestSuite *TestSuite::instance()
{
    return Singleton<TestSuite>::instance();
}

TestSuite::TestSuite():
    testCaseFailureCount_{0},
    testCaseSkipCount_{0},
    totalFailureCount_{0}
{}

TestSuite::~TestSuite()
{}

bool TestSuite::init(int argc, char **argv)
{
    testCaseFailureCount_ = 0;
    testCaseSkipCount_ = 0;
    totalFailureCount_ = 0;
    execPath_ = argv[0];
    name_ = execPath_->baseName();

    try {
        VariantMap options;
        options->insert("report", "txt");

        Arguments{argc, argv}->read(options);

        String reportType = options->value("report");
        if (reportType == "txt")
            report_ = TxtTestReport::create();
        else if (reportType == "xml")
            report_ = XmlTestReport::create(stdOut());
        else {
            ferr("Unsupported report type \"%%\"\n\n") << reportType;
            throw HelpRequest{};
        }
    }
    catch (HelpRequest &) {
        fout(
            "Usage: TEST_CASE [OPTION]...\n"
            "Execute test suite and generate test report.\n"
            "\n"
            "Options:\n"
            "  -report=txt  select output format (\"xml\" or \"txt\")\n"
        );
        return false;
    }

    return true;
}

bool TestSuite::verify(bool condition, const String &description, const String &conditionText, const String &codePath, int codeLine)
{
    if (!condition) {
        ++totalFailureCount_;
        ++currentTestCase_->failureCount_;
    }
    report_->verify(currentTestCase_, condition, conditionText, codePath, codeLine);
    return condition;
}

int TestSuite::run(int argc, char **argv)
{
    if (!init(argc, argv)) return 1;

    bool interactive = stdOut()->isatty();

    report_->beginTestSuite(this);

    for (int i = 0; i < testCases_->count(); ++i)
    {
        Ref<TestCase> testCase = testCases_->at(i);
        testCase->assertionCount_ = 0;
        testCase->failureCount_ = 0;

        if (testCase->skip()) {
            report_->skipTestCase(testCase);
            ++testCaseSkipCount_;
            continue;
        }

        report_->beginTestCase(testCase);

        SystemStream outSaved, errSaved;
        File outFile, errFile;
        if (report_->captureOutput()) {
            outSaved->duplicate(stdOut());
            errSaved->duplicate(stdErr());
            outFile = TempFile{};
            errFile = TempFile{};
            outFile->duplicateTo(stdOut());
            errFile->duplicateTo(stdErr());
        }

        if (interactive) {
            currentTestCase_ = testCase;
            testCase->run();
        }
        else {
            try {
                currentTestCase_ = testCase;
                testCase->run();
            }
            catch (Exception &ex) {
                String typeName = typeid(ex).name();
                {
                    int status = 0;
                    char *buf = abi::__cxa_demangle(typeName, 0, 0, &status);
                    if (status == 0) typeName = buf;
                    ::free(buf);
                }
                testCase->caughtException_ = true;
                report_->error(testCase, typeName, ex.message());
            }
            catch (...) {
                testCase->caughtException_ = true;
                report_->error(testCase, "", "");
            }
        }

        String outText, errText;
        if (report_->captureOutput()) {
            outSaved->duplicateTo(stdOut());
            errSaved->duplicateTo(stdErr());
            outText = outFile->map();
            errText = errFile->map();
        }

        report_->endTestCase(testCase, outText, errText);

        if (testCase->failureCount() > 0 || testCase->caughtException()) ++testCaseFailureCount_;
    }

    report_->endTestSuite(this);

    return testCaseFailureCount_;
}

}} // namespace cc::testing
