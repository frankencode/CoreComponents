/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Sha256HashSink>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "Sha256OneBlock",
        []{
            String message = "abc";
            String hash = hex(sha256(message)).breakUp(8).join(" ");
            fout("SHA-256 of \"%%\": %%") << message << hash << nl;
            CC_VERIFY(hash == "BA7816BF 8F01CFEA 414140DE 5DAE2223 B00361A3 96177A9C B410FF61 F20015AD");
        }
    };

    TestCase {
        "Sha256TwoBlock",
        []{
            String message = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
            String hash = hex(sha256(message)).breakUp(8).join(" ");
            fout("SHA-256 of \"%%\": %%") << message << hash << nl;
            CC_VERIFY(hash == "248D6A61 D20638B8 E5C02693 0C3E6039 A33CE459 64FF2167 F6ECEDD4 19DB06C1");
        }
    };

    return TestSuite{argc, argv}.run();
}
