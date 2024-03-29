/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/AesBlockCipher>
#include <cc/testing>

namespace cc::aes { Bytes keyExpansion(const Bytes &key, int Nr = -1); }

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "AesKeyExpansion",
        []{
            CC_CHECK(
                aes::keyExpansion({ // 128 bit
                    0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
                    0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
                })
                .endsWith({
                    0xb6, 0x63, 0x0c, 0xa6
                })
            );

            CC_CHECK(
                aes::keyExpansion({ // 196 bit
                    0x8e, 0x73, 0xb0, 0xf7, 0xdA, 0x0e, 0x64, 0x52,
                    0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5,
                    0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b
                })
                .endsWith({
                    0x01, 0x00, 0x22, 0x02
                })
            );

            CC_CHECK(
                aes::keyExpansion({ // 256 bit
                    0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
                    0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
                    0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
                    0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
                })
                .endsWith({
                    0x70, 0x6c, 0x63, 0x1e
                })
            );
        }
    };

    TestCase {
        "AesCipherSimpleMessage",
        []{
            struct Util {
                static void printState(const char *name, const Bytes &s)
                {
                    fout() << name << ":" << nl;
                    for (int r = 0; r < 4; ++r) {
                        Format ln;
                        for (int c = 0; c < 4; ++c)
                            ln << hex(s.byteAt(4 * c + r), 2);
                        fout() << "    " << ln.join(' ') << nl;
                    }
                    fout() << nl;
                }

                static void testCipher(const Bytes &k, const Bytes &p, const Bytes &c)
                {
                    fout("Testing AES-%%...") << k.count() * 8 << nl << nl;
                    AesBlockCipher codec{k};
                    Bytes c2 = Bytes::allocate(codec.blockSize());
                    Bytes p2 = Bytes::allocate(codec.blockSize());
                    codec.encode(p, &c2);
                    printState("  Plaintext", p);
                    printState("  Enciphered text", c2);
                    codec.decode(c, &p2);
                    printState("  Deciphered text", p2);
                    fout("  ");
                    CC_CHECK(c == c2 && p == p2);
                    fout() << nl;
                }
            };

            Util::testCipher(
                /*k =*/ {
                    0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
                    0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
                },
                /*p =*/ {
                    0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d,
                    0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34
                },
                /*c =*/ {
                    0x39, 0x25, 0x84, 0x1d, 0x02, 0xdc, 0x09, 0xfb,
                    0xdc, 0x11, 0x85, 0x97, 0x19, 0x6a, 0x0b, 0x32
                }
            );

            Util::testCipher(
                /*k =*/ {
                    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
                },
                /*p =*/ {
                    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                    0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
                },
                /*c =*/ {
                    0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
                    0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a
                }
            );

            Util::testCipher(
                /*k =*/ {
                    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17
                },
                /*p =*/ {
                    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                    0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
                },
                /*c =*/ {
                    0xdd, 0xa9, 0x7c, 0xa4, 0x86, 0x4c, 0xdf, 0xe0,
                    0x6e, 0xaf, 0x70, 0xa0, 0xec, 0x0d, 0x71, 0x91
                }
            );

            Util::testCipher(
                /*k =*/ {
                    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
                },
                /*p =*/ {
                    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                    0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
                },
                /*c =*/ {
                    0x8e, 0xa2, 0xb7, 0xca, 0x51, 0x67, 0x45, 0xbf,
                    0xea, 0xfc, 0x49, 0x90, 0x4b, 0x49, 0x60, 0x89
                }
            );
        }
    };

    return TestSuite{argc, argv}.run();
}
