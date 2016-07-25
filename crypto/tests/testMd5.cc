/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/crypto/Md5>

using namespace cc;
using namespace cc::testing;
using namespace cc::crypto;

class Md5Examples: public TestCase
{
    void run()
    {
        Ref<StringList> tests = StringList::create()
            << ""
            << "a"
            << "abc"
            << "message digest"
            << "abcdefghijklmnopqrstuvwxyz"
            << "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
            << "123456789012345678901234567890123456789" "01234567890123456789012345678901234567890";

        Ref<StringList> results = StringList::create()
            << "d41d8cd98f00b204e9800998ecf8427e"
            << "0cc175b9c0f1b6a831c399e269772661"
            << "900150983cd24fb0d6963f7d28e17f72"
            << "f96b697d7cb7938d525a2f31aaf161d0"
            << "c3fcd3d76192e4007dfb496cca67e13b"
            << "d174ab98d277d9f5a5611c2c9f419d9f"
            << "57edf4a22be3c955ac49da2e2107b67a";

        for (int i = 0; i < tests->count(); ++i) {
            String requiredSum = results->at(i);
            String sum = md5(tests->at(i))->toHex();
            fout("MD5 of \"%%\":") << tests->at(i) << nl;
            fout() << "  " << requiredSum << " (required)" << nl;
            fout() << "  " << sum << " (delivered)" << nl;
            CC_VERIFY(sum == requiredSum);
        }
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(Md5Examples);

    return TestSuite::instance()->run(argc, argv);
}
