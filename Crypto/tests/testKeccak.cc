/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Keccak>
#include <cc/HexDump>
#include <cc/testing>

namespace cc { template class Keccak<1024>; }

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "Keccak_NIST_SHA3-512_1600",
        []{
            Bytes data = Bytes::allocate(200);
            data.fill(0xA3);

            Keccak<1024, Sha3Padding> keccak;
            for (long offset = 0; keccak.absorb({data, offset}); offset += keccak.FeedRate);

            String expectedHashText {
                "E7 6D FA D2 20 84 A8 B1 46 7F CF 2F FA 58 36 1B "
                "EC 76 28 ED F5 F3 FD C0 E4 80 5D C4 8C AE EC A8 "
                "1B 7C 13 C3 0A DF 52 A3 65 95 84 73 9A 2D F4 6B "
                "E5 89 C5 1C A1 A4 A8 41 6D F6 54 5A 1C E8 BA 00"
            };

            Bytes outHash = Bytes::allocate(64);
            keccak.squeeze({&outHash});

            String outHashText = hex(outHash).breakUp(2).join(" ");
            CC_INSPECT("\n" + outHashText.breakUp(3*16).join("\n"));

            fout() << nl;

            CC_CHECK(expectedHashText == outHashText);
        }
    };

    return TestSuite{argc, argv}.run();
}
