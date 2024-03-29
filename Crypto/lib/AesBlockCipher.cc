/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/AesBlockCipher>
#include <cassert>
#include <cstdint>

namespace cc {

namespace aes {

enum { Nb = 4, Ns = Nb * 4 };

uint8_t sbox[256] = {
    0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
    0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
    0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
    0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
    0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
    0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
    0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
    0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
    0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
    0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
    0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
    0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
    0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
    0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
    0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
    0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};

uint8_t invSbox[256] = {
    0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38, 0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB,
    0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87, 0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB,
    0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D, 0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E,
    0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2, 0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25,
    0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92,
    0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA, 0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84,
    0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A, 0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06,
    0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02, 0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B,
    0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA, 0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73,
    0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85, 0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E,
    0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89, 0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B,
    0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20, 0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4,
    0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31, 0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F,
    0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D, 0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF,
    0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0, 0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, 0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D
};

inline void subBytes(InOut<Bytes> s_)
{
    uint8_t *s = s_->bytes();

    for (int i = 0; i < Ns; ++i)
        s[i] = sbox[s[i]];
}

inline void invSubBytes(InOut<Bytes> s_)
{
    uint8_t *s = s_->bytes();

    for (int i = 0; i < Ns; ++i)
        s[i] = invSbox[s[i]];
}

inline void shiftRows(InOut<Bytes> s_)
{
    uint8_t *s = s_->bytes();

    {    // second row
        uint8_t h1 = s[0x1];
        s[0x1] = s[0x5];
        s[0x5] = s[0x9];
        s[0x9] = s[0xD];
        s[0xD] = h1;
    }

    {    // third row
        uint8_t h2 = s[0x2];
        uint8_t h6 = s[0x6];
        s[0x2] = s[0xA];
        s[0x6] = s[0xE];
        s[0xA] = h2;
        s[0xE] = h6;
    }

    {    // forth row
        uint8_t h3 = s[0x3];
        uint8_t h7 = s[0x7];
        uint8_t hB = s[0xB];
        s[0x3] = s[0xF];
        s[0x7] = h3;
        s[0xB] = h7;
        s[0xF] = hB;
    }
}

inline void invShiftRows(InOut<Bytes> s_)
{
    uint8_t *s = s_->bytes();

    {    // second row
        uint8_t hD = s[0xD];
        s[0xD] = s[0x9];
        s[0x9] = s[0x5];
        s[0x5] = s[0x1];
        s[0x1] = hD;
    }
    {    // third row
        uint8_t hE = s[0xE];
        uint8_t hA = s[0xA];
        s[0xE] = s[0x6];
        s[0xA] = s[0x2];
        s[0x6] = hE;
        s[0x2] = hA;
    }
    {    // forth row
        uint8_t hF = s[0xF];
        uint8_t hB = s[0xB];
        uint8_t h7 = s[0x7];
        s[0xF] = s[0x3];
        s[0xB] = hF;
        s[0x7] = hB;
        s[0x3] = h7;
    }
}

static uint8_t mul2(uint8_t a)
{
    bool cf = (a & 0x80);
    a <<= 1;
    if (cf) a ^= 0x1b;
    return a;
}

static uint8_t mul3(uint8_t a)
{
    return mul2(a) ^ a;
}

static uint8_t mul(uint8_t a, uint8_t b)
{
    int c = 0, h = a, f = 1;
    while (f <= b) {
        if (b & f) c ^= h;
        h = mul2(h);
        f <<= 1;
    }
    return c;
}

static void mixColumns(InOut<Bytes> s_)
{
    uint8_t *s = s_->bytes();

    for (int c = 0; c < Nb; ++c) {
        uint8_t *sc = s + c * Nb;
        uint8_t h0 = sc[0];
        uint8_t h1 = sc[1];
        uint8_t h2 = sc[2];
        uint8_t h3 = sc[3];
        sc[0] = h2 ^ h3 ^ mul2(h0) ^ mul3(h1);
        sc[1] = h0 ^ h3 ^ mul2(h1) ^ mul3(h2);
        sc[2] = h0 ^ h1 ^ mul2(h2) ^ mul3(h3);
        sc[3] = h1 ^ h2 ^ mul2(h3) ^ mul3(h0);
    }
}

static void invMixColumns(InOut<Bytes> s_)
{
    uint8_t *s = s_->bytes();

    for (int c = 0; c < Nb; ++c) {
        uint8_t *sc = s + c * Nb;
        uint8_t h0 = sc[0];
        uint8_t h1 = sc[1];
        uint8_t h2 = sc[2];
        uint8_t h3 = sc[3];
        sc[0] = mul(h3, 0x9) ^ mul(h1, 0xB) ^ mul(h2, 0xD) ^ mul(h0, 0xE);
        sc[1] = mul(h0, 0x9) ^ mul(h2, 0xB) ^ mul(h3, 0xD) ^ mul(h1, 0xE);
        sc[2] = mul(h1, 0x9) ^ mul(h3, 0xB) ^ mul(h0, 0xD) ^ mul(h2, 0xE);
        sc[3] = mul(h2, 0x9) ^ mul(h0, 0xB) ^ mul(h1, 0xD) ^ mul(h3, 0xE);
    }
}

static void addRoundKey(InOut<Bytes> s, const Bytes &w, int r)
{
    for (int c = 0; c < 4; ++c)
        s->wordAt(c) ^= w.wordAt(r * Nb + c);
}

static uint32_t subWord(uint32_t w)
{
    union {
        uint32_t h;
        uint8_t b[4];
    };
    h = w;
    b[0] = sbox[b[0]];
    b[1] = sbox[b[1]];
    b[2] = sbox[b[2]];
    b[3] = sbox[b[3]];
    return h;
}

static uint32_t rotWord(uint32_t w)
{
    return (w >> 8) | (w << 24);
}

static int numRounds(int Nk)
{
    if (Nk == 4) return 10;
    else if (Nk == 6) return 12;
    /*else if (Nk == 8)*/ return 14;
}

static uint32_t word(uint8_t a0, uint8_t a1, uint8_t a2, uint8_t a3)
{
    return uint32_t(a0) |
        (uint32_t(a1) << 8) |
        (uint32_t(a2) << 16) |
        (uint32_t(a3) << 24);
}

static uint8_t rCon(int i)
{
    uint8_t h = 1;
    for (int j = 0; j < i - 1; ++j) {
        bool cf = h & 0x80;
        h <<= 1;
        if (cf) h ^= 0x1B;
    }
    return h;
}

Bytes keyExpansion(const Bytes &key, int Nr)
{
    if (Nr <= 0) Nr = numRounds(key.count() / 4);
    Bytes w = Bytes::allocate(Ns * (Nr + 1));

    const int Nk = key.count() / 4;
    int i = 0;

    for (; i < Nk; ++i) {
        w.wordAt(i) =
            word(
                key.byteAt(4 * i),
                key.byteAt(4 * i + 1),
                key.byteAt(4 * i + 2),
                key.byteAt(4 * i + 3)
            );
    }

    for (const int n = w.count() / 4; i < n; ++i) {
        uint32_t h = w.wordAt(i - 1);
        if (i % Nk == 0) {
            h = subWord(rotWord(h)) ^ rCon(i / Nk);
        }
        else if (Nk > 6 && i % Nk == 4) {
            h = subWord(h);
        }
        w.wordAt(i) = w.wordAt(i - Nk) ^ h;
    }

    return w;
}

} // namespace aes

using namespace aes;

struct AesBlockCipher::State: public BlockCipher::State
{
    State(const Bytes &key):
        BlockCipher::State{AesBlockCipher::BlockSize},
        Nk_{static_cast<int>(key.count() / 4)},
        Nr_{numRounds(Nk_)},
        s_{Bytes::allocate(AesBlockCipher::BlockSize)},
        w_{keyExpansion(key, Nr_)}
    {
        assert(key.count() == 16 || key.count() == 24 || key.count() == 32);
    }

    void encode(const Bytes &p, Out<Bytes> c) override
    {
        assert(p.count() == AesBlockCipher::BlockSize);
        assert(c != None{} && c->count() == AesBlockCipher::BlockSize);

        p.copyTo(&s_);

        addRoundKey(&s_, w_, 0);

        for (int r = 1; r < Nr_; ++r) {
            subBytes(&s_);
            shiftRows(&s_);
            mixColumns(&s_);
            addRoundKey(&s_, w_, r);
        }

        subBytes(&s_);
        shiftRows(&s_);
        addRoundKey(&s_, w_, Nr_);

        s_.copyTo(&c);
    }

    void decode(const Bytes &c, Out<Bytes> p) override
    {
        assert(c.count() == AesBlockCipher::BlockSize);
        assert(p != None{} && p->count() == AesBlockCipher::BlockSize);

        c.copyTo(&s_);

        addRoundKey(&s_, w_, Nr_);

        for (int r = Nr_ - 1; r > 0; --r) {
            invShiftRows(&s_);
            invSubBytes(&s_);
            addRoundKey(&s_, w_, r);
            invMixColumns(&s_);
        }

        invShiftRows(&s_);
        invSubBytes(&s_);
        addRoundKey(&s_, w_, 0);

        s_.copyTo(&p);
    }

    const int Nk_, Nr_;
    Bytes s_;
    Bytes w_;
};

AesBlockCipher::AesBlockCipher(const Bytes &key):
    BlockCipher{new State{key}}
{}

} // namespace