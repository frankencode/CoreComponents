/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Keccak>
#include <cc/HexDump>
#include <cc/Map>
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


            static constexpr uint8_t shuffle_[25] = { 1, 10, 7, 11, 17, 18, 3, 5, 16, 8, 21, 24, 4, 15, 23, 19, 13, 12, 2, 20, 14, 22, 9, 6, 1 };

            Map<int,int> shifts;

            for (int t = 0; t < 24; ++t) {
                shifts.insert(shuffle_[t], (t + 1) * (t + 2) / 2);
            }

            for (int t = 0; t < 24; ++t) {
                fout("%%, ") << shifts[t].value() % 64;
            }
            fout("\n");

            /*int l = 0;
            Map<int, int> map;
            for (int y = 0; y < 5; ++y) {
                for (int x = 0; x < 5; ++x) {
                    int xs = (x + 3 * y) % 5;
                    int ls = xs + 5 * x;
                    // fout() << ls << ", ";
                    if (l != ls) {
                        fout("%% => %%;\n") << ls << l;
                        map.insert(ls, l);
                    }
                    ++l;
                }
            }

            fout() << nl;*/
        }
    };

    return TestSuite{argc, argv}.run();
}
