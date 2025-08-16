/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Base64>
#include <cc/testing>

int main(int argc, char* argv[])
{
    using namespace cc;

    TestCase {
        "Base64ShortStrings",
        []{
            {
                CC_CHECK(fromBase64("TWFu") == "Man");
                CC_CHECK(base64("Man") == "TWFu");
            }
            {
                CC_CHECK(fromBase64("SGVsbG8gd29ybGQ=") == "Hello world");
                CC_CHECK(base64("Hello world") == "SGVsbG8gd29ybGQ=");
            }

            CC_CHECK(base64("") == "");
            CC_CHECK(fromBase64("") == "");
        }
    };

    TestCase {
        "Base64_RFC4648",
        []{
            CC_CHECK_EQUALS(base64(String{""}), String{""});
            CC_CHECK_EQUALS(base64(String{"f"}), String{"Zg=="});
            CC_CHECK_EQUALS(base64(String{"fo"}), String{"Zm8="});
            CC_CHECK_EQUALS(base64(String{"foo"}), String{"Zm9v"});
            CC_CHECK_EQUALS(base64(String{"foob"}), String{"Zm9vYg=="});
            CC_CHECK_EQUALS(base64(String{"fooba"}), String{"Zm9vYmE="});
            CC_CHECK_EQUALS(base64(String{"foobar"}), String{"Zm9vYmFy"});

            CC_CHECK_EQUALS(fromBase64(String{""}), String{""});
            CC_CHECK_EQUALS(fromBase64(String{"Zg=="}), String{"f"});
            CC_CHECK_EQUALS(fromBase64(String{"Zm8="}), String{"fo"});
            CC_CHECK_EQUALS(fromBase64(String{"Zm9v"}), String{"foo"});
            CC_CHECK_EQUALS(fromBase64(String{"Zm9vYg=="}), String{"foob"});
            CC_CHECK_EQUALS(fromBase64(String{"Zm9vYmE="}), String{"fooba"});
            CC_CHECK_EQUALS(fromBase64(String{"Zm9vYmFy"}), String{"foobar"});
        }
    };

    return TestSuite{argc, argv}.run();
}
