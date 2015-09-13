#include <flux/stdio>
#include "TtyTestReport.h"

namespace flux {
namespace testing {

TtyTestReport::TtyTestReport()
{}

bool TtyTestReport::captureOutput() const
{
    return false;
}

void TtyTestReport::beginTestSuite(TestSuite *testSuite)
{
    fout("#### %%\n") << testSuite->name();
}

void TtyTestReport::skipTestCase(TestCase *testCase)
{
    fout("## skipped: test case %%\n") << testCase->name();
}

void TtyTestReport::beginTestCase(TestCase *testCase)
{
    fout("## start: test case %%\n") << testCase->name();
}

void TtyTestReport::verify(TestCase *testCase, bool condition, String message, String codePath, int codeLine)
{
    if (!condition)
        fout("! FAILED: %%:%%: FLUX_VERIFY(%%)\n")
        << codePath->fileName() << codeLine << message;
}

void TtyTestReport::error(TestCase *testCase, String type, String message)
{
    fout("! EXCEPTION %%: %%\n") << type << message;
}

void TtyTestReport::endTestCase(TestCase *testCase, String outText, String errText)
{
    if (testCase->passed())
        fout("## passed: test case %%\n") << testCase->name();
    else
        fout("!! FAILED: test case %%\n") << testCase->name();
}

void TtyTestReport::endTestSuite(TestSuite *testSuite)
{
    fout("#### %% passed, %% failed, %% skipped (%%)\n")
        << testSuite->testCaseCount()
        << testSuite->testCaseFailureCount()
        << testSuite->testCaseSkipCount()
        << testSuite->name();
}

}} // namespace flux::testing
