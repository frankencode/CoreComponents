/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Keccak>

namespace cc {

static constexpr uint64_t keccak_round_word[24] = {
    0x1ull,
    0x8082ull,
    0x800000000000808Aull,
    0x8000000080008000ull,
    0x808Bull,
    0x80000001ull,
    0x8000000080008081ull,
    0x8000000000008009ull,
    0x8Aull,
    0x88ull,
    0x80008009ull,
    0x8000000Aull,
    0x8000808Bull,
    0x800000000000008Bull,
    0x8000000000008089ull,
    0x8000000000008003ull,
    0x8000000000008002ull,
    0x8000000000000080ull,
    0x800Aull,
    0x800000008000000Aull,
    0x8000000080008081ull,
    0x8000000000008080ull,
    0x80000001ull,
    0x8000000080008008ull
};

void keccak_permutate(uint64_t *state)
{
    using std::rotl;
    using std::swap;

    uint64_t a, b, c, d, e;

    for (int ir = 0; ir < 24; ++ir)
    {
        a = state[0];
        b = state[1];
        c = state[2];
        d = state[3];
        e = state[4];

        a ^= state[5];
        b ^= state[6];
        c ^= state[7];
        d ^= state[8];
        e ^= state[9];

        a ^= state[10];
        b ^= state[11];
        c ^= state[12];
        d ^= state[13];
        e ^= state[14];

        a ^= state[15];
        b ^= state[16];
        c ^= state[17];
        d ^= state[18];
        e ^= state[19];

        a ^= state[20];
        b ^= state[21];
        c ^= state[22];
        d ^= state[23];
        e ^= state[24];

        state[ 0] ^= e;
        state[ 1] ^= a;
        state[ 2] ^= b;
        state[ 3] ^= c;
        state[ 4] ^= d;
        state[ 5] ^= e;
        state[ 6] ^= a;
        state[ 7] ^= b;
        state[ 8] ^= c;
        state[ 9] ^= d;
        state[10] ^= e;
        state[11] ^= a;
        state[12] ^= b;
        state[13] ^= c;
        state[14] ^= d;
        state[15] ^= e;
        state[16] ^= a;
        state[17] ^= b;
        state[18] ^= c;
        state[19] ^= d;
        state[20] ^= e;
        state[21] ^= a;
        state[22] ^= b;
        state[23] ^= c;
        state[24] ^= d;

        a = rotl(a, 1);
        b = rotl(b, 1);
        c = rotl(c, 1);
        d = rotl(d, 1);
        e = rotl(e, 1);

        state[ 0] ^= b;
        state[ 1] = rotl(state[ 1] ^ c,  1);
        state[ 2] = rotl(state[ 2] ^ d, 62);
        state[ 3] = rotl(state[ 3] ^ e, 28);
        state[ 4] = rotl(state[ 4] ^ a, 27);
        state[ 5] = rotl(state[ 5] ^ b, 36);
        state[ 6] = rotl(state[ 6] ^ c, 44);
        state[ 7] = rotl(state[ 7] ^ d,  6);
        state[ 8] = rotl(state[ 8] ^ e, 55);
        state[ 9] = rotl(state[ 9] ^ a, 20);
        state[10] = rotl(state[10] ^ b,  3);
        state[11] = rotl(state[11] ^ c, 10);
        state[12] = rotl(state[12] ^ d, 43);
        state[13] = rotl(state[13] ^ e, 25);
        state[14] = rotl(state[14] ^ a, 39);
        state[15] = rotl(state[15] ^ b, 41);
        state[16] = rotl(state[16] ^ c, 45);
        state[17] = rotl(state[17] ^ d, 15);
        state[18] = rotl(state[18] ^ e, 21);
        state[19] = rotl(state[19] ^ a,  8);
        state[20] = rotl(state[20] ^ b, 18);
        state[21] = rotl(state[21] ^ c,  2);
        state[22] = rotl(state[22] ^ d, 61);
        state[23] = rotl(state[23] ^ e, 56);
        state[24] = rotl(state[24] ^ a, 14);

        a = state[1];
        swap(state[10], a);
        swap(state[ 7], a);
        swap(state[11], a);
        swap(state[17], a);
        swap(state[18], a);
        swap(state[ 3], a);
        swap(state[ 5], a);
        swap(state[16], a);
        swap(state[ 8], a);
        swap(state[21], a);
        swap(state[24], a);
        swap(state[ 4], a);
        swap(state[15], a);
        swap(state[23], a);
        swap(state[19], a);
        swap(state[13], a);
        swap(state[12], a);
        swap(state[ 2], a);
        swap(state[20], a);
        swap(state[14], a);
        swap(state[22], a);
        swap(state[ 9], a);
        swap(state[ 6], a);
        swap(state[ 1], a);

        a = state[0];
        b = state[1];
        state[0] ^= (~state[1] & state[2]);
        state[1] ^= (~state[2] & state[3]);
        state[2] ^= (~state[3] & state[4]);
        state[3] ^= (~state[4] & a       );
        state[4] ^= (~a        & b       );

        a = state[5];
        b = state[6];
        state[5] ^= (~state[6] & state[7]);
        state[6] ^= (~state[7] & state[8]);
        state[7] ^= (~state[8] & state[9]);
        state[8] ^= (~state[9] & a       );
        state[9] ^= (~a        & b       );

        a = state[10];
        b = state[11];
        state[10] ^= (~state[11] & state[12]);
        state[11] ^= (~state[12] & state[13]);
        state[12] ^= (~state[13] & state[14]);
        state[13] ^= (~state[14] & a        );
        state[14] ^= (~a         & b        );

        a = state[15];
        b = state[16];
        state[15] ^= (~state[16] & state[17]);
        state[16] ^= (~state[17] & state[18]);
        state[17] ^= (~state[18] & state[19]);
        state[18] ^= (~state[19] & a        );
        state[19] ^= (~a         & b        );

        a = state[20];
        b = state[21];
        state[20] ^= (~state[21] & state[22]);
        state[21] ^= (~state[22] & state[23]);
        state[22] ^= (~state[23] & state[24]);
        state[23] ^= (~state[24] & a        );
        state[24] ^= (~a         & b        );

        state[0] ^= keccak_round_word[ir];
    }
}

} // namespace cc
