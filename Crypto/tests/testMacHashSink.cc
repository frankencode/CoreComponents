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
#include <cc/Sha256HashSink>
#include <cc/Sha512HashSink>
#include <cc/Sha3HashSink>
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
                String mac = hex(macSink.finish(), 'a');

                fout() << "HMAC-MD5_0 = 0x" << mac << nl;

                CC_VERIFY(mac == "9294727a3638bb1c13f48ef8158bfc9d");
            }
            {
                String key = "Jefe";
                String text = "what do ya want for nothing?";

                MacHashSink<Md5HashSink> macSink{key};
                macSink.write(text);
                String mac = hex(macSink.finish(), 'a');

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
                String mac = hex(macSink.finish(), 'a');

                fout() << "HMAC-SHA1_0 = 0x" << mac << nl;

                CC_VERIFY(mac == "b617318655057264e28bc0b6fb378c8ef146be00");
            }
            {
                String key = "Jefe";
                String text = "what do ya want for nothing?";

                MacHashSink<Sha1HashSink> macSink{key};
                macSink.write(text);
                String mac = hex(macSink.finish(), 'a');

                fout() << "HMAC-SHA1_1 = 0x" << mac << nl;

                CC_VERIFY(mac == "effcdf6ae5eb2fa2d27416d5f184df9c259a7c79");
            }
        }
    };

    TestCase {
        "MacHashSha256Nist1",
        []{
            Bytes key { Bytes::allocate(64) };
            for (int i = 0; i < key.count(); ++i) key[i] = i;
            String text { "Sample message for keylen=blocklen" };

            MacHashSink<Sha256HashSink> macSink{key};
            macSink.write(text);

            String mac = hex(macSink.finish()).breakUp(8).join(" ");

            fout() << "MAC = " << mac << nl;
            CC_VERIFY(mac == "8BB9A1DB 9806F20D F7F77B82 138C7914 D174D59E 13DC4D01 69C9057B 133E1D62");
        }
    };

    TestCase {
        "MacHashSha256Nist2",
        []{
            Bytes key { Bytes::allocate(32) };
            for (int i = 0; i < key.count(); ++i) key[i] = i;
            String text { "Sample message for keylen<blocklen" };

            MacHashSink<Sha256HashSink> macSink{key};
            macSink.write(text);

            String mac = hex(macSink.finish()).breakUp(8).join(" ");

            fout() << "MAC = " << mac << nl;
            CC_VERIFY(mac == "A28CF431 30EE696A 98F14A37 678B56BC FCBDD9E5 CF69717F ECF5480F 0EBDF790");
        }
    };

    TestCase {
        "MacHashSha512Nist1",
        []{
            Bytes key { Bytes::allocate(128) };
            for (int i = 0; i < key.count(); ++i) key[i] = i;
            String text { "Sample message for keylen=blocklen" };

            MacHashSink<Sha512HashSink> macSink{key};
            macSink.write(text);

            String mac = hex(macSink.finish()).breakUp(8).join(" ");

            fout() << "MAC = " << mac << nl;
            CC_VERIFY(mac == "FC25E240 658CA785 B7A811A8 D3F7B4CA 48CFA26A 8A366BF2 CD1F836B 05FCB024 BD368530 81811D6C EA4216EB AD79DA1C FCB95EA4 586B8A0C E356596A 55FB1347");
        }
    };

    TestCase {
        "MacHashSha512Nist2",
        []{
            Bytes key { Bytes::allocate(64) };
            for (int i = 0; i < key.count(); ++i) key[i] = i;
            String text { "Sample message for keylen<blocklen" };

            MacHashSink<Sha512HashSink> macSink{key};
            macSink.write(text);

            String mac = hex(macSink.finish()).breakUp(8).join(" ");

            fout() << "MAC = " << mac << nl;
            CC_VERIFY(mac == "FD44C18B DA0BB0A6 CE0E82B0 31BF2818 F6539BD5 6EC00BDC 10A8A2D7 30B3634D E2545D63 9B0F2CF7 10D0692C 72A1896F 1F211C2B 922D1A96 C392E07E 7EA9FEDC");
        }
    };

    TestCase {
        "MacHashSha256Wikipedia",
        []{
            String key = "key";
            String text = "The quick brown fox jumps over the lazy dog";

            MacHashSink<Sha256HashSink> macSink{key};
            macSink.write(text);
            String mac = hex(macSink.finish(), 'a');

            fout("HMAC_SHA256(\"%%\", \"%%\") = 0x%%") << key << text << mac << nl;

            CC_VERIFY(mac == "f7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8");
        }
    };

    TestCase {
        "MacHashSha512Wikipedia",
        []{
            String key = "key";
            String text = "The quick brown fox jumps over the lazy dog";

            MacHashSink<Sha512HashSink> macSink{key};
            macSink.write(text);
            String mac = hex(macSink.finish(), 'a');

            fout("HMAC_SHA512(\"%%\", \"%%\") = 0x%%") << key << text << mac << nl;

            CC_VERIFY(mac == "b42af09057bac1e2d41708e48a902e09b5ff7f12ab428a4fe86653c73dd248fb82f948a549f7b791a5b41915ee4d1ec3935357e4e2317250d0372afa2ebeeb3a");
        }
    };

    TestCase {
        "MacHashSha3512Nist1",
        []{
            Bytes key { Bytes::allocate(64) };
            for (int i = 0; i < key.count(); ++i) key[i] = i;

            String text { "Sample message for keylen<blocklen" };

            MacHashSink<Sha3HashSink<512>> macSink{key};
            macSink.write(text);

            String mac = hex(macSink.finish(), 'a').breakUp(8).join(" ");

            fout() << "MAC = " << mac << nl;

            CC_VERIFY(
                mac
                ==
                "4efd629d 6c71bf86 162658f2 9943b1c3 "
                "08ce27cd fa6db0d9 c3ce8176 3f9cbce5 "
                "f7ebe986 8031db1a 8f8eb7b6 b95e5c5e "
                "3f657a89 96c86a2f 6527e307 f0213196"
            );
        }
    };

    TestCase {
        "MacHashSha3512Nist2",
        []{
            Bytes key { Bytes::allocate(72) };
            for (int i = 0; i < key.count(); ++i) key[i] = i;

            String text { "Sample message for keylen=blocklen" };

            MacHashSink<Sha3HashSink<512>> macSink{key};
            macSink.write(text);

            String mac = hex(macSink.finish(), 'a').breakUp(8).join(" ");

            fout() << "MAC = " << mac << nl;

            CC_VERIFY(
                mac
                ==
                "544e257e a2a3e5ea 19a590e6 a24b724c "
                "e6327757 723fe275 1b75bf00 7d80f6b3 "
                "60744bf1 b7a88ea5 85f9765b 47911976 "
                "d3191cf8 3c039f5f fab0d29c c9d9b6da"
            );
        }
    };

    TestCase {
        "MacHashSha3384Nist1",
        []{
            Bytes key { Bytes::allocate(48) };
            for (int i = 0; i < key.count(); ++i) key[i] = i;

            String text { "Sample message for keylen<blocklen" };

            MacHashSink<Sha3HashSink<384>> macSink{key};
            macSink.write(text);

            String mac = hex(macSink.finish(), 'a').breakUp(8).join(" ");

            fout() << "MAC = " << mac << nl;

            CC_VERIFY(
                mac
                ==
                "d588a3c5 1f3f2d90 6e8298c1 199aa8ff "
                "62962181 27f6b38a 90b6afe2 c5617725 "
                "bc99987f 79b22a55 7b6520db 710b7f42"
            );
        }
    };

    TestCase {
        "MacHashSha3384Nist2",
        []{
            Bytes key { Bytes::allocate(104) };
            for (int i = 0; i < key.count(); ++i) key[i] = i;

            String text { "Sample message for keylen=blocklen" };

            MacHashSink<Sha3HashSink<384>> macSink{key};
            macSink.write(text);

            String mac = hex(macSink.finish(), 'a').breakUp(8).join(" ");

            fout() << "MAC = " << mac << nl;

            CC_VERIFY(
                mac
                ==
                "a27d24b5 92e8c8cb f6d4ce6f c5bf62d8 "
                "fc98bf2d 486640d9 eb8099e2 4047837f "
                "5f3bffbe 92dcce90 b4ed5b1e 7e44fa90"
            );
        }
    };

    TestCase {
        "MacHashSha3256Nist1",
        []{
            Bytes key { Bytes::allocate(32) };
            for (int i = 0; i < key.count(); ++i) key[i] = i;

            String text { "Sample message for keylen<blocklen" };

            MacHashSink<Sha3HashSink<256>> macSink{key};
            macSink.write(text);

            String mac = hex(macSink.finish(), 'a').breakUp(8).join(" ");

            fout() << "MAC = " << mac << nl;

            CC_VERIFY(
                mac
                ==
                "4fe8e202 c4f058e8 dddc23d8 c34e4673 "
                "43e23555 e24fc2f0 25d598f5 58f67205"
            );
        }
    };

    TestCase {
        "MacHashSha3256Nist2",
        []{
            Bytes key { Bytes::allocate(136) };
            for (int i = 0; i < key.count(); ++i) key[i] = i;

            String text { "Sample message for keylen=blocklen" };

            MacHashSink<Sha3HashSink<256>> macSink{key};
            macSink.write(text);

            String mac = hex(macSink.finish(), 'a').breakUp(8).join(" ");

            fout() << "MAC = " << mac << nl;

            CC_VERIFY(
                mac
                ==
                "68b94e2e 538a9be4 103bebb5 aa016d47 "
                "961d4d1a a9060613 13b557f8 af2c3faa"
            );
        }
    };

    TestCase {
        "MacHashSha3224Nist1",
        []{
            Bytes key { Bytes::allocate(28) };
            for (int i = 0; i < key.count(); ++i) key[i] = i;

            String text { "Sample message for keylen<blocklen" };

            MacHashSink<Sha3HashSink<224>> macSink{key};
            macSink.write(text);

            String mac = hex(macSink.finish(), 'a').breakUp(8).join(" ");

            fout() << "MAC = " << mac << nl;

            CC_VERIFY(
                mac
                ==
                "332cfd59 347fdb8e 576e7726 0be4aba2 "
                "d6dc5311 7b3bfb52 c6d18c04"
            );
        }
    };

    TestCase {
        "MacHashSha3224Nist2",
        []{
            Bytes key { Bytes::allocate(144) };
            for (int i = 0; i < key.count(); ++i) key[i] = i;

            String text { "Sample message for keylen=blocklen" };

            MacHashSink<Sha3HashSink<224>> macSink{key};
            macSink.write(text);

            String mac = hex(macSink.finish(), 'a').breakUp(8).join(" ");

            fout() << "MAC = " << mac << nl;

            CC_VERIFY(
                mac
                ==
                "d8b733bc f66c644a 12323d56 4e24dcf3 "
                "fc75f231 f3b67968 359100c7"
            );
        }
    };

    return TestSuite{argc, argv}.run();
}
