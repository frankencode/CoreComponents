/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/crypto/AesCipher>

namespace cc {
namespace crypto {
namespace aes {
String keyExpansion(const ByteArray *k, int Nr);
}}}

using namespace cc;
using namespace cc::testing;
using namespace cc::crypto;
using namespace cc::crypto::aes;

class TestKeyExpansion: public TestCase
{
    static bool testKeyExpansion128()
    {
        const uint8_t k[16] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
        String w = keyExpansion(ByteArray::copy((char*)k, sizeof(k)), -1);
        return
            w->byteAt(w->count() - 4) == 0xb6 &&
            w->byteAt(w->count() - 3) == 0x63 &&
            w->byteAt(w->count() - 2) == 0x0c &&
            w->byteAt(w->count() - 1) == 0xa6;
    }

    static bool testKeyExpansion196()
    {
        const uint8_t k[24] = {
            0x8e, 0x73, 0xb0, 0xf7, 0xdA, 0x0e, 0x64, 0x52, 0xc8, 0x10, 0xf3, 0x2b,
            0x80, 0x90, 0x79, 0xe5, 0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b
        };
        String w = keyExpansion(ByteArray::copy((char*)k, sizeof(k)), -1);
        return
            w->at(w->count() - 4) == 0x01 &&
            w->at(w->count() - 3) == 0x00 &&
            w->at(w->count() - 2) == 0x22 &&
            w->at(w->count() - 1) == 0x02;
    }

    static bool testKeyExpansion256()
    {
        const uint8_t k[32] = {
            0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
            0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
        };
        String w = keyExpansion(ByteArray::copy((char*)k, sizeof(k)), -1);
        return
            w->at(w->count() - 4) == 0x70 &&
            w->at(w->count() - 3) == 0x6c &&
            w->at(w->count() - 2) == 0x63 &&
            w->at(w->count() - 1) == 0x1e;
    }

    void run() {
        CC_VERIFY(testKeyExpansion128());
        CC_VERIFY(testKeyExpansion196());
        CC_VERIFY(testKeyExpansion256());
    }
};

class AesExamples: public TestCase
{
    void printState(const char *name, const String &s)
    {
        fout() << name << ":" << nl;
        for (int r = 0; r < 4; ++r) {
            Format ln;
            for (int c = 0; c < 4; ++c)
                ln << hex(s->byteAt(4 * c + r), 2);
            fout() << "  " << ln->join(" ") << nl;
        }
    }

    bool testCipher(String k, String p, String c)
    {
        fout("Testing AES-%%...") << k->count() * 8 << nl;
        Ref<AesCipher> codec = AesCipher::create(k);
        String c2(codec->blockSize());
        String p2(codec->blockSize());
        codec->encode(p, mutate(c2));
        printState("plaintext", p);
        printState("enciphered text", c2);
        codec->decode(c, mutate(p2));
        printState("deciphered text", p2);
        return c == c2 && p == p2;
    }

    void run()
    {
        CC_VERIFY(
            testCipher(
                /*k =*/"\x2b\x7e\x15\x16\x28\xae\xd2\xa6\xab\xf7\x15\x88\x09\xcf\x4f\x3c",
                /*p =*/"\x32\x43\xf6\xa8\x88\x5a\x30\x8d\x31\x31\x98\xa2\xe0\x37\x07\x34",
                /*c =*/"\x39\x25\x84\x1d\x02\xdc\x09\xfb\xdc\x11\x85\x97\x19\x6a\x0b\x32"
            )
        );

        CC_VERIFY(
            testCipher(
                /*k =*/String("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f", 16),
                /*p =*/String("\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff", 16),
                /*c =*/"\x69\xc4\xe0\xd8\x6a\x7b\x04\x30\xd8\xcd\xb7\x80\x70\xb4\xc5\x5a"
            )
        );

        CC_VERIFY(
            testCipher(
                /*k =*/String("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17", 24),
                /*p =*/String("\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff", 16),
                /*c =*/"\xdd\xa9\x7c\xa4\x86\x4c\xdf\xe0\x6e\xaf\x70\xa0\xec\x0d\x71\x91"
            )
        );

        CC_VERIFY(
            testCipher(
                /*k =*/String("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f", 32),
                /*p =*/String("\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff", 16),
                /*c =*/"\x8e\xa2\xb7\xca\x51\x67\x45\xbf\xea\xfc\x49\x90\x4b\x49\x60\x89"
            )
        );
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(TestKeyExpansion);
    CC_TESTSUITE_ADD(AesExamples);

    return TestSuite::instance()->run(argc, argv);
}

#if 0
void printKeySchedule(ByteArray *w)
{
    for (int i = 0; i < w->count() / 4; ++i) {
        uint8_t b0 = w->byteAt(i * 4);
        uint8_t b1 = w->byteAt(i * 4 + 1);
        uint8_t b2 = w->byteAt(i * 4 + 2);
        uint8_t b3 = w->byteAt(i * 4 + 3);
        fout() << right(dec(i), 2) << ": " << hex(b0, 2) << hex(b1, 2) << hex(b2, 2) << hex(b3, 2) << nl;
    }
}
#endif
