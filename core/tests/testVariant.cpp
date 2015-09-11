/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/testing/TestSuite>
#include <flux/stdio>
#include <flux/Variant>

using namespace flux;
using namespace flux::testing;

class Comparisms: public TestCase
{
    void run()
    {
        Variant x = 1;
        Variant y = "2.";
        Variant z = true;
        Variant a = String("abc");

        FLUX_VERIFY(!(x < y));
        FLUX_VERIFY(!(x == y));
        FLUX_VERIFY(!(x < z));
        FLUX_VERIFY(x == z);
        FLUX_VERIFY(y < a);
        FLUX_VERIFY(y != a);
        FLUX_VERIFY(!(Variant() < x));
        FLUX_VERIFY(!(x == Variant()));
        FLUX_VERIFY(!(Variant() < Variant()));
        FLUX_VERIFY(Variant() == Variant());
    }
};

int main(int argc, char **argv)
{
    FLUX_TESTSUITE_ADD(Comparisms);

    return testSuite()->run(argc, argv);
}
