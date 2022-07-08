/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Md5HashSink>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "Md5HashSimple",
        []{
            List<String> tests {
                "",
                "a",
                "abc",
                "message digest",
                "abcdefghijklmnopqrstuvwxyz",
                "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
                "123456789012345678901234567890123456789" "01234567890123456789012345678901234567890"
            };

            List<String> results {
                "d41d8cd98f00b204e9800998ecf8427e",
                "0cc175b9c0f1b6a831c399e269772661",
                "900150983cd24fb0d6963f7d28e17f72",
                "f96b697d7cb7938d525a2f31aaf161d0",
                "c3fcd3d76192e4007dfb496cca67e13b",
                "d174ab98d277d9f5a5611c2c9f419d9f",
                "57edf4a22be3c955ac49da2e2107b67a"
            };

            for (int i = 0; i < tests.count(); ++i) {
                String requiredSum = results.at(i);
                String sum = hex(md5(tests.at(i)));
                fout("MD5 of \"%%\":") << tests.at(i) << nl;
                fout() << "  " << requiredSum << " (required)" << nl;
                fout() << "  " << sum << " (delivered)" << nl;
                CC_VERIFY(sum == requiredSum);
                fout() << nl;
            }
        }
    };

    return TestSuite{argc, argv}.run();
}
