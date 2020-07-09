/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/debug>
#include <cc/crypto/Md5Sink>
#include <cc/crypto/Sha1Sink>
#include <cc/crypto/HashMacSink>

using namespace cc;
using namespace cc::testing;
using namespace cc::crypto;

class SimpleMd5MacTest: public TestCase
{
    void run()
    {
        Md5Sink md5Sink;

        {
            String key = String::allocate(16);;
            for (int i = 0; i < key->count(); ++i)
                mutate(key)->byteAt(i) = 0x0B;

            String text = "Hi There";

            HashMacSink macSink{md5Sink, key};
            macSink->write(text);
            String mac = macSink->finish()->toHex();

            fout() << "HMAC-MD5_0 = 0x" << mac << nl;

            CC_VERIFY(mac == "9294727a3638bb1c13f48ef8158bfc9d");
        }
        {
            String key = "Jefe";
            String text = "what do ya want for nothing?";

            HashMacSink macSink{md5Sink, key};
            macSink->write(text);
            String mac = macSink->finish()->toHex();

            fout() << "HMAC-MD5_1 = 0x" << mac << nl;

            CC_VERIFY(mac == "750c783e6ab0b503eaa86e310a5db738");
        }
    }
};

class SimpleSha1MacTest: public TestCase
{
    void run()
    {
        Sha1Sink sha1Sink;

        {
            String key = String::allocate(20);;
            for (int i = 0; i < key->count(); ++i)
                mutate(key)->byteAt(i) = 0x0B;

            String text = "Hi There";

            HashMacSink macSink{sha1Sink, key};
            macSink->write(text);
            String mac = macSink->finish()->toHex();

            fout() << "HMAC-SHA1_0 = 0x" << mac << nl;

            CC_VERIFY(mac == "b617318655057264e28bc0b6fb378c8ef146be00");
        }
        {
            String key = "Jefe";
            String text = "what do ya want for nothing?";

            HashMacSink macSink{sha1Sink, key};
            macSink->write(text);
            String mac = macSink->finish()->toHex();

            fout() << "HMAC-SHA1_1 = 0x" << mac << nl;

            CC_VERIFY(mac == "effcdf6ae5eb2fa2d27416d5f184df9c259a7c79");
        }
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(SimpleMd5MacTest);
    CC_TESTSUITE_ADD(SimpleSha1MacTest);

    return TestSuite::instance()->run(argc, argv);
}
