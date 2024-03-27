/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TokiTheme>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "ThemeLoading",
        []{
            const char *themeName = "ClassicWhite";
            String themePath;
            try {
                themePath = TokiTheme::locate(themeName);
            }
            catch (...)
            {
                CC_DEBUG << "Failed to locate theme " << themeName;
                return;
            }
            CC_INSPECT(themePath);
            TokiTheme theme{themePath};
            CC_INSPECT(theme.defaultPalette().textStyle().ink());
            CC_INSPECT(theme.defaultPalette().textStyle().paper());
        }
    };

    return TestSuite{argc, argv}.run();
}
