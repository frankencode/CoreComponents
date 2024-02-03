/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/base64>
#include <cc/testing>

int main(int argc, char* argv[])
{
    using namespace cc;

    TestCase {
        "Base64ShortStrings",
        []{
            String test[] = { "Man", "Hello world", "", "1" };
            const int testCount = sizeof(test) / sizeof(test[0]);

            for (int i = 0; i < testCount; ++i) {
                String a = test[i], b = base64Encode(a);
                fout("base64(\"%%\") = \"%%\"\n") << a << b;
                CC_VERIFY(base64Decode(b) == a);
            }

            CC_INSPECT(hex(base64Decode(String{"FfxLSnBiza9x7YQb"})));
        }
    };

    return TestSuite{argc, argv}.run();
}
