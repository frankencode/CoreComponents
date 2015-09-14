/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <typeinfo>
#include <cxxabi.h> // __cxa_demangle
#include <stdlib.h> // free(3)
#include <flux/Exception>
#include <flux/Singleton>
#include <flux/stdio>
#include <flux/File>
#include <flux/Arguments>
#include <flux/testing/XmlTestReport>
#include <flux/testing/TxtTestReport>
#include <flux/testing/TestCase>
#include <flux/testing/TestSuite>

namespace flux {
namespace testing {

TestSuite::TestSuite():
    testCases_(TestCases::create()),
    testCaseFailureCount_(0),
    testCaseSkipCount_(0),
    totalFailureCount_(0)
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
        Ref<Arguments> arguments = Arguments::parse(argc, argv);

        Ref<VariantMap> options = VariantMap::create();
        options->insert("report", "txt");
        arguments->validate(options);
        arguments->override(options);

        Ref<StringList> items = arguments->items();

        String reportType = options->value("report");
        if (reportType == "txt")
            report_ = TxtTestReport::create();
        else if (reportType == "xml")
            report_ = XmlTestReport::create(stdOut());
        else {
            ferr("Unsupported report type \"%%\"\n\n") << reportType;
            throw HelpError();
        }
    }
    catch (HelpError &) {
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

bool TestSuite::verify(bool condition, String description, String conditionText, String codePath, int codeLine)
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

        Ref<SystemStream> outSaved, errSaved;
        Ref<File> outFile, errFile;
        if (report_->captureOutput()) {
            outSaved = SystemStream::duplicate(stdOut());
            errSaved = SystemStream::duplicate(stdErr());
            outFile = File::temp();
            errFile = File::temp();
            FileUnlinkGuard outGuard(outFile->path());
            FileUnlinkGuard errGuard(errFile->path());
            outFile->duplicateTo(stdOut());
            errFile->duplicateTo(stdErr());
        }

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

TestSuite *testSuite() { return Singleton<TestSuite>::instance(); }

}} // namespace flux::testing
