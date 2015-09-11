/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/testing/TestSuite>
#include <flux/stdio>
#include <flux/toki/Theme>

using namespace flux;
using namespace flux::testing;
using namespace flux::toki;

class ThemeLoading: public TestCase
{
    void run() {
        Theme::load("ClassicWhite");
    }
};

int main(int argc, char** argv)
{
    FLUX_TESTSUITE_ADD(ThemeLoading);

    return testSuite()->run(argc, argv);
}
