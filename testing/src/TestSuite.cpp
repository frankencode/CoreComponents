/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <typeinfo>
#include <flux/Exception>
#include <flux/Singleton>
#include <flux/stdio>
#include <flux/File>
#include "XmlTestReport.h"
#include "TestCase.h"
#include "TestSuite.h"

namespace flux {
namespace testing {

TestSuite::TestSuite():
    testCases_(TestCases::create()),
    testCaseFailureCount_(0),
    totalFailureCount_(0)
{}

TestSuite::~TestSuite()
{}

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
    name_ = String(argv[0])->baseName();
    totalFailureCount_ = 0;
    testCaseFailureCount_ = 0;

    report_ = XmlTestReport::create(stdOut());
    report_->beginTestSuite(this);

    for (int i = 0; i < testCases_->count(); ++i)
    {
        Ref<TestCase> testCase = testCases_->at(i);
        testCase->assertionCount_ = 0;
        testCase->failureCount_ = 0;

        report_->beginTestCase(testCase);

        if (testCase->skip()) {
            report_->skip(testCase);
            report_->endTestCase(testCase);
            continue;
        }

        Ref<SystemStream> outSaved = SystemStream::duplicate(stdOut());
        Ref<SystemStream> errSaved = SystemStream::duplicate(stdErr());
        Ref<File> outFile = File::temp();
        Ref<File> errFile = File::temp();
        FileUnlinkGuard outGuard(outFile->path());
        FileUnlinkGuard errGuard(errFile->path());
        outFile->duplicateTo(stdOut());
        errFile->duplicateTo(stdErr());

        try {
            currentTestCase_ = testCase;
            testCase->run();
        }
        catch (Exception &ex) {
            report_->error(testCase, typeid(ex).name(), ex.message());
        }

        outSaved->duplicateTo(stdOut());
        errSaved->duplicateTo(stdErr());

        report_->endTestCase(testCase, outFile->map(), errFile->map());

        if (testCase->failureCount() > 0) ++testCaseFailureCount_;
    }

    report_->endTestSuite(this);

    return testCaseFailureCount_;
}

TestSuite *testSuite() { return Singleton<TestSuite>::instance(); }

}} // namespace flux::testing
