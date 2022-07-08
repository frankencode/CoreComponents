/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/BlockCipherSink>
#include <cc/BlockCipherSource>
#include <cc/AesBlockCipher>
#include <cc/MemoryStream>
#include <cc/NullStream>
#include <cc/Random>
#include <cc/math>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    struct Test {
        static String cycle(const String &text, const BlockCipher &cipher)
        {
            Bytes buffer = Bytes::allocate(upToNext(cipher.blockSize(), text.count()));
            {
                MemoryStream stream{buffer};
                BlockCipherSink{stream, cipher}.write(text);
            }
            String text2 = String::allocate(text.count());
            {
                MemoryStream stream{buffer};
                BlockCipherSource{stream, cipher}.read(&text2);
            }
            return text2;
        }
    };

    TestCase {
        "AesBlockCipherStreamBlockBoundary",
        []{
            Bytes key {
                0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
                0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
            };
            AesBlockCipher cipher{key};
            String text = "0123456789,0123456789,0123456789";
            for (int n = 1; n <= text.count(); ++n) {
                String text1 = text.copy(0, n);
                String text2 = Test::cycle(text1, cipher);
                fout() << text1 << " >> " << text2 << nl;
                CC_VERIFY(text1 == text2);
            }
        }
    };

    TestCase {
        "AesBlockCipherStreamBufferBoundary",
        []{
            Bytes key {
                0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
                0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
            };
            AesBlockCipher cipher{key};
            String text = String::allocate(0x1001);
            {
                Random random;
                for (int i = 0; i < text.count(); ++i) {
                    text.byteAt(i) = random.get() & 0xFF;
                }
            }
            {
                String text1 = text.copy(0, text.count() - 1);
                String text2 = Test::cycle(text1, cipher);
                CC_VERIFY(text1 == text2);
            }
            {
                String text1 = text;
                String text2 = Test::cycle(text1, cipher);
                CC_VERIFY(text1 == text2);
            }
        }
    };

    return TestSuite{argc, argv}.run();
}
