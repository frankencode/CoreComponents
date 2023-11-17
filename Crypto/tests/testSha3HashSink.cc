/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Sha3HashSink>
#include <cc/HexDump>
#include <cc/testing>

namespace cc { template class Sha3HashSink<512>; }

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "Sha3HashSink_NIST_SHA3-224_1600",
        []{
            Bytes data = Bytes::allocate(200);
            data.fill(0xA3);

            Sha3HashSink<224> sink;
            sink.write(data);
            Bytes outHash = sink.finish();

            String expectedHashText {
                "93 76 81 6A BA 50 3F 72 F9 6C E7 EB 65 AC 09 5D "
                "EE E3 BE 4B F9 BB C2 A1 CB 7E 11 E0"
            };

            String outHashText = hex(outHash).breakUp(2).join(" ");
            CC_INSPECT("\n" + outHashText.breakUp(3*16).join("\n"));

            fout() << nl;

            CC_CHECK(expectedHashText == outHashText);
        }
    };

    TestCase {
        "Sha3HashSink_NIST_SHA3-256_1600",
        []{
            Bytes data = Bytes::allocate(200);
            data.fill(0xA3);

            Sha3HashSink<256> sink;
            sink.write(data);
            Bytes outHash = sink.finish();

            String expectedHashText {
                "79 F3 8A DE C5 C2 03 07 A9 8E F7 6E 83 24 AF BF "
                "D4 6C FD 81 B2 2E 39 73 C6 5F A1 BD 9D E3 17 87"
            };

            String outHashText = hex(outHash).breakUp(2).join(" ");
            CC_INSPECT("\n" + outHashText.breakUp(3*16).join("\n"));

            fout() << nl;

            CC_CHECK(expectedHashText == outHashText);
        }
    };

    TestCase {
        "Sha3HashSink_NIST_SHA3-384_1600",
        []{
            Bytes data = Bytes::allocate(200);
            data.fill(0xA3);

            Sha3HashSink<384> sink;
            sink.write(data);
            Bytes outHash = sink.finish();

            String expectedHashText {
                "18 81 DE 2C A7 E4 1E F9 5D C4 73 2B 8F 5F 00 2B "
                "18 9C C1 E4 2B 74 16 8E D1 73 26 49 CE 1D BC DD "
                "76 19 7A 31 FD 55 EE 98 9F 2D 70 50 DD 47 3E 8F"
            };

            String outHashText = hex(outHash).breakUp(2).join(" ");
            CC_INSPECT("\n" + outHashText.breakUp(3*16).join("\n"));

            fout() << nl;

            CC_CHECK(expectedHashText == outHashText);
        }
    };

    TestCase {
        "Sha3HashSink_NIST_SHA3-512_1600",
        []{
            Bytes data = Bytes::allocate(200);
            data.fill(0xA3);

            Sha3HashSink<512> sink;
            sink.write(data);
            Bytes outHash = sink.finish();

            String expectedHashText {
                "E7 6D FA D2 20 84 A8 B1 46 7F CF 2F FA 58 36 1B "
                "EC 76 28 ED F5 F3 FD C0 E4 80 5D C4 8C AE EC A8 "
                "1B 7C 13 C3 0A DF 52 A3 65 95 84 73 9A 2D F4 6B "
                "E5 89 C5 1C A1 A4 A8 41 6D F6 54 5A 1C E8 BA 00"
            };

            String outHashText = hex(outHash).breakUp(2).join(" ");
            CC_INSPECT("\n" + outHashText.breakUp(3*16).join("\n"));

            fout() << nl;

            CC_CHECK(expectedHashText == outHashText);
        }
    };

    return TestSuite{argc, argv}.run();
}
