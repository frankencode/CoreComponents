/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/TxtTestReport>
#include <cc/TestSuite>
#include <cc/stdio>

namespace cc {

bool TxtTestReport::captureOutput() const
{
    return false;
}

void TxtTestReport::beginTestSuite(const TestSuite &testSuite)
{}

void TxtTestReport::beginTestCase(const TestCase &testCase)
{
    fout("%%: %%::run()\n") << TestSuite{}.name() << testCase.name();
    fout("...........................................................\n");
}

void TxtTestReport::verify(const TestCase &testCase, bool condition, const String &message, const String &codePath, int codeLine)
{
    if (!condition) {
        fout("FAILED: %%:%%: CC_VERIFY(%%)\n")
            << codePath.fileName() << codeLine << message;
    }
}

void TxtTestReport::error(const TestCase &, const String &type, const String &message)
{
    fout("CAUGHT EXCEPTION %%: %%\n") << type << message;
}

void TxtTestReport::endTestCase(const TestCase &testCase, const String &, const String &)
{
    fout("...........................................................\n");
    fout("%%\n\n") << (testCase.passed() ? "PASSED" : "FAILED");
}

void TxtTestReport::skipTestCase(const TestCase &testCase)
{}

void TxtTestReport::endTestSuite(const TestSuite &testSuite)
{}

} // namespace cc
