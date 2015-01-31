/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stdio>
#include <flux/toki/Theme>

using namespace flux;
using namespace flux::toki;

int main()
{
    Ref<Theme> theme = Theme::load("ClassicWhite");
    return 0;
}
