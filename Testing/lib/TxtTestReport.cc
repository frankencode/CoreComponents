/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TxtTestReport>
#include <cc/TestSuite>
#include <cc/stdio>

namespace cc {

struct TxtTestReport::State: public TestReport::State
{
    bool captureOutput() const override
    {
        return false;
    }

    void beginTestSuite(const TestSuite &testSuite) override
    {}

    void beginTestCase(const TestCase &testCase) override
    {
        fout("%%.%%\n") << TestSuite{}.name() << testCase.name();
        fout("...........................................................\n");
    }

    void verify(const TestCase &testCase, bool condition, const String &message, const String &codePath, int codeLine) override
    {
        if (!condition) {
            fout("FAILED: %%:%%: CC_VERIFY(%%)\n")
                << codePath.fileName() << codeLine << message;
        }
    }

    void error(const TestCase &testCase, const String &type, const String &message) override
    {
        fout("CAUGHT EXCEPTION %%: %%\n") << type << message;
    }

    void endTestCase(const TestCase &testCase, const String &outText = "", const String &errText = "") override
    {
        fout("...........................................................\n");
        fout("%%\n\n") << (testCase.passed() ? "PASSED" : "FAILED");
    }

    void skipTestCase(const TestCase &testCase) override
    {}

    void endTestSuite(const TestSuite &testSuite) override
    {}
};

TxtTestReport::TxtTestReport():
    TestReport{new State}
{}

} // namespace cc
