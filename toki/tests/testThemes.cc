/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/toki/Theme>

using namespace cc;
using namespace cc::testing;
using namespace cc::toki;

class ThemeLoading: public TestCase
{
    void run() {
        Theme::load("ClassicWhite");
    }
};

int main(int argc, char** argv)
{
    CC_TESTSUITE_ADD(ThemeLoading);

    return TestSuite::instance()->run(argc, argv);
}
