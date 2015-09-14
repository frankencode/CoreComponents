#include <flux/stdio>
#include "TxtTestReport.h"

namespace flux {
namespace testing {

TxtTestReport::TxtTestReport()
{}

bool TxtTestReport::captureOutput() const
{
    return false;
}

void TxtTestReport::beginTestSuite(TestSuite *testSuite)
{}

void TxtTestReport::skipTestCase(TestCase *testCase)
{
    fout("Test case \"%%\": SKIPPED\n") << testCase->name();
    fout("...........................................................\n");
}

void TxtTestReport::beginTestCase(TestCase *testCase)
{
    fout("%%: %%::run()\n") << testSuite()->name() << testCase->name();
    fout("...........................................................\n");
}

void TxtTestReport::verify(TestCase *testCase, bool condition, String message, String codePath, int codeLine)
{
    if (!condition)
        fout("FAILED: %%:%%: FLUX_VERIFY(%%)\n")
        << codePath->fileName() << codeLine << message;
}

void TxtTestReport::error(TestCase *testCase, String type, String message)
{
    fout("CAUGHT EXCEPTION %%: %%\n") << type << message;
}

void TxtTestReport::endTestCase(TestCase *testCase, String outText, String errText)
{
    fout("...........................................................\n");
    fout("%%\n\n") << (testCase->passed() ? "PASSED" : "FAILED");
}

void TxtTestReport::endTestSuite(TestSuite *testSuite)
{}

}} // namespace flux::testing
