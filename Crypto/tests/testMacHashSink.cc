/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/MacHashSink>
#include <cc/Md5HashSink>
#include <cc/Sha1HashSink>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "MacHashMd5Simple",
        []{
            {
                Bytes key = Bytes::allocate(16);;
                for (uint8_t &x: key) x = 0x0B;

                String text = "Hi There";

                MacHashSink<Md5HashSink> macSink{key};
                macSink.write(text);
                String mac = hex(macSink.finish());

                fout() << "HMAC-MD5_0 = 0x" << mac << nl;

                CC_VERIFY(mac == "9294727a3638bb1c13f48ef8158bfc9d");
            }
            {
                String key = "Jefe";
                String text = "what do ya want for nothing?";

                MacHashSink<Md5HashSink> macSink{key};
                macSink.write(text);
                String mac = hex(macSink.finish());

                fout() << "HMAC-MD5_1 = 0x" << mac << nl;

                CC_VERIFY(mac == "750c783e6ab0b503eaa86e310a5db738");
            }
        }
    };

    TestCase {
        "MacHashSha1Simple",
        []{
            {
                Bytes key = Bytes::allocate(20);;
                for (uint8_t &x: key) x = 0x0B;

                String text = "Hi There";

                MacHashSink<Sha1HashSink> macSink{key};
                macSink.write(text);
                String mac = hex(macSink.finish());

                fout() << "HMAC-SHA1_0 = 0x" << mac << nl;

                CC_VERIFY(mac == "b617318655057264e28bc0b6fb378c8ef146be00");
            }
            {
                String key = "Jefe";
                String text = "what do ya want for nothing?";

                MacHashSink<Sha1HashSink> macSink{key};
                macSink.write(text);
                String mac = hex(macSink.finish());

                fout() << "HMAC-SHA1_1 = 0x" << mac << nl;

                CC_VERIFY(mac == "effcdf6ae5eb2fa2d27416d5f184df9c259a7c79");
            }
        }
    };


    return TestSuite{argc, argv}.run();
}
