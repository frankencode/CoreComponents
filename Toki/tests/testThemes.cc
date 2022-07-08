/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/toki/Theme>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;
    using namespace cc::toki;

    TestCase {
        "ThemeLoading",
        []{
            Theme theme{"ClassicWhite"};
            CC_INSPECT(theme.defaultPalette().textStyle().ink());
            CC_INSPECT(theme.defaultPalette().textStyle().paper());
        }
    };

    return TestSuite{argc, argv}.run();
}
