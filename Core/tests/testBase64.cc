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

    TestCase {
        "Base64_RFC4648",
        []{
            CC_CHECK_EQUALS(base64Encode(String{""}), String{""});
            CC_CHECK_EQUALS(base64Encode(String{"f"}), String{"Zg=="});
            CC_CHECK_EQUALS(base64Encode(String{"fo"}), String{"Zm8="});
            CC_CHECK_EQUALS(base64Encode(String{"foo"}), String{"Zm9v"});
            CC_CHECK_EQUALS(base64Encode(String{"foob"}), String{"Zm9vYg=="});
            CC_CHECK_EQUALS(base64Encode(String{"fooba"}), String{"Zm9vYmE="});
            CC_CHECK_EQUALS(base64Encode(String{"foobar"}), String{"Zm9vYmFy"});

            CC_CHECK_EQUALS(base64Decode(String{""}), String{""});
            CC_CHECK_EQUALS(base64Decode(String{"Zg=="}), String{"f"});
            CC_CHECK_EQUALS(base64Decode(String{"Zm8="}), String{"fo"});
            CC_CHECK_EQUALS(base64Decode(String{"Zm9v"}), String{"foo"});
            CC_CHECK_EQUALS(base64Decode(String{"Zm9vYg=="}), String{"foob"});
            CC_CHECK_EQUALS(base64Decode(String{"Zm9vYmE="}), String{"fooba"});
            CC_CHECK_EQUALS(base64Decode(String{"Zm9vYmFy"}), String{"foobar"});
        }
    };

    return TestSuite{argc, argv}.run();
}
