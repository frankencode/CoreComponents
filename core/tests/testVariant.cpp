/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stdio>
#include <flux/check>
#include <flux/Variant>

using namespace flux;

int main()
{
    Variant x = 1;
    Variant y = "2.";
    Variant z = true;
    Variant a = String("abc");

    check(!(x < y));
    check(!(x == y));
    check(!(x < z));
    check(x == z);
    check(y < a);
    check(y != a);
    check(!(Variant() < x));
    check(!(x == Variant()));
    check(!(Variant() < Variant()));
    check(Variant() == Variant());

    return 0;
}
