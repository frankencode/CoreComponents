/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Md5HashSink>
#include <cc/bits>
#include <cstring>

namespace cc {

struct Md5HashSink::State: public CryptoHashSink::State
{

    static constexpr int BlockSize = 64;
    static constexpr int AuxSize = 0x4000 + BlockSize;

    State()
    {
        static_assert((AuxSize % BlockSize) == 0);
    }

    int blockSize() const override { return BlockSize; }

    void write(const Bytes &data, long fill = -1) override
    {
        const uint8_t *src = data.bytes();
        for (
            int srcLeft = fill < 0 ? data.count() : fill;
            srcLeft > 0;
        ) {
            while ((auxFill_ < aux_.count()) && (srcLeft > 0)) {
                int n = aux_.count() - auxFill_;
                if (n > srcLeft) n = srcLeft;
                std::memcpy(aux_.bytes() + auxFill_, src, n);
                src += n;
                srcLeft -= n;
                auxFill_ += n;
                bytesFeed_ += n;
            }
            if (auxFill_ % BlockSize == 0) {
                consume();
            }
        }
    }

    void consume()
    {
        uint32_t *m = aux_.words();
        for (int i = 0, nc = auxFill_ / BlockSize; i < nc; ++i) {
            // select 16 dwords x[0], x[1] ... x[15] and
            // ensure value of the dwords is independent from system endianess
            uint32_t *x = m + i * 16;
            if (localEndian() == ByteOrder::BigEndian) {
                for (int j = 0; j < 16; ++j)
                    x[j] = swap32(x[j]);
            }
            // load state
            uint32_t a = a_;
            uint32_t b = b_;
            uint32_t c = c_;
            uint32_t d = d_;
            // round 1
            r1(x,a,b,c,d, 0, 7,T1 );r1(x,d,a,b,c, 1,12,T2 );r1(x,c,d,a,b, 2,17,T3 );r1(x,b,c,d,a, 3,22,T4 );
            r1(x,a,b,c,d, 4, 7,T5 );r1(x,d,a,b,c, 5,12,T6 );r1(x,c,d,a,b, 6,17,T7 );r1(x,b,c,d,a, 7,22,T8 );
            r1(x,a,b,c,d, 8, 7,T9 );r1(x,d,a,b,c, 9,12,T10);r1(x,c,d,a,b,10,17,T11);r1(x,b,c,d,a,11,22,T12);
            r1(x,a,b,c,d,12, 7,T13);r1(x,d,a,b,c,13,12,T14);r1(x,c,d,a,b,14,17,T15);r1(x,b,c,d,a,15,22,T16);
            // round 2
            r2(x,a,b,c,d, 1, 5,T17);r2(x,d,a,b,c, 6, 9,T18);r2(x,c,d,a,b,11,14,T19);r2(x,b,c,d,a, 0,20,T20);
            r2(x,a,b,c,d, 5, 5,T21);r2(x,d,a,b,c,10, 9,T22);r2(x,c,d,a,b,15,14,T23);r2(x,b,c,d,a, 4,20,T24);
            r2(x,a,b,c,d, 9, 5,T25);r2(x,d,a,b,c,14, 9,T26);r2(x,c,d,a,b, 3,14,T27);r2(x,b,c,d,a, 8,20,T28);
            r2(x,a,b,c,d,13, 5,T29);r2(x,d,a,b,c, 2, 9,T30);r2(x,c,d,a,b, 7,14,T31);r2(x,b,c,d,a,12,20,T32);
            // round 3
            r3(x,a,b,c,d, 5, 4,T33);r3(x,d,a,b,c, 8,11,T34);r3(x,c,d,a,b,11,16,T35);r3(x,b,c,d,a,14,23,T36);
            r3(x,a,b,c,d, 1, 4,T37);r3(x,d,a,b,c, 4,11,T38);r3(x,c,d,a,b, 7,16,T39);r3(x,b,c,d,a,10,23,T40);
            r3(x,a,b,c,d,13, 4,T41);r3(x,d,a,b,c, 0,11,T42);r3(x,c,d,a,b, 3,16,T43);r3(x,b,c,d,a, 6,23,T44);
            r3(x,a,b,c,d, 9, 4,T45);r3(x,d,a,b,c,12,11,T46);r3(x,c,d,a,b,15,16,T47);r3(x,b,c,d,a, 2,23,T48);
            // round 4
            r4(x,a,b,c,d, 0, 6,T49);r4(x,d,a,b,c, 7,10,T50);r4(x,c,d,a,b,14,15,T51);r4(x,b,c,d,a, 5,21,T52);
            r4(x,a,b,c,d,12, 6,T53);r4(x,d,a,b,c, 3,10,T54);r4(x,c,d,a,b,10,15,T55);r4(x,b,c,d,a, 1,21,T56);
            r4(x,a,b,c,d, 8, 6,T57);r4(x,d,a,b,c,15,10,T58);r4(x,c,d,a,b, 6,15,T59);r4(x,b,c,d,a,13,21,T60);
            r4(x,a,b,c,d, 4, 6,T61);r4(x,d,a,b,c,11,10,T62);r4(x,c,d,a,b, 2,15,T63);r4(x,b,c,d,a, 9,21,T64);
            // increment
            a_ += a;
            b_ += b;
            c_ += c;
            d_ += d;
        }
        auxFill_ = 0;
    }

    Bytes finish() override
    {
        /** manually feed the padding and message size
          */
        aux_.byteAt(auxFill_++) = 0x80;
        while ((auxFill_ % 64) != 56) {
            aux_.byteAt(auxFill_++) = 0x00;
        }
        uint64_t b = bytesFeed_ << 3; // message length in bits
        for (int i = 0; i < 8; ++i) {
            aux_.byteAt(auxFill_++) = b >> (i * 8);
        }
        consume();

        /** serialize the message digest
          */
        Bytes h = Bytes::allocate(Md5HashSink::HashSize);
        int k = 0;
        for (int i = 0; i < 4; ++i) h.at(k++) = a_ >> (8 * i);
        for (int i = 0; i < 4; ++i) h.at(k++) = b_ >> (8 * i);
        for (int i = 0; i < 4; ++i) h.at(k++) = c_ >> (8 * i);
        for (int i = 0; i < 4; ++i) h.at(k++) = d_ >> (8 * i);
        return h;
    }

    /** Ti = uint32_t(4294967296. * fabs(sin(i))) for i = 1..64
      */
    static constexpr uint32_t T1  = 0xD76AA478;
    static constexpr uint32_t T2  = 0xE8C7B756;
    static constexpr uint32_t T3  = 0x242070DB;
    static constexpr uint32_t T4  = 0xC1BDCEEE;
    static constexpr uint32_t T5  = 0xF57C0FAF;
    static constexpr uint32_t T6  = 0x4787C62A;
    static constexpr uint32_t T7  = 0xA8304613;
    static constexpr uint32_t T8  = 0xFD469501;
    static constexpr uint32_t T9  = 0x698098D8;
    static constexpr uint32_t T10 = 0x8B44F7AF;
    static constexpr uint32_t T11 = 0xFFFF5BB1;
    static constexpr uint32_t T12 = 0x895CD7BE;
    static constexpr uint32_t T13 = 0x6B901122;
    static constexpr uint32_t T14 = 0xFD987193;
    static constexpr uint32_t T15 = 0xA679438E;
    static constexpr uint32_t T16 = 0x49B40821;
    static constexpr uint32_t T17 = 0xF61E2562;
    static constexpr uint32_t T18 = 0xC040B340;
    static constexpr uint32_t T19 = 0x265E5A51;
    static constexpr uint32_t T20 = 0xE9B6C7AA;
    static constexpr uint32_t T21 = 0xD62F105D;
    static constexpr uint32_t T22 = 0x02441453;
    static constexpr uint32_t T23 = 0xD8A1E681;
    static constexpr uint32_t T24 = 0xE7D3FBC8;
    static constexpr uint32_t T25 = 0x21E1CDE6;
    static constexpr uint32_t T26 = 0xC33707D6;
    static constexpr uint32_t T27 = 0xF4D50D87;
    static constexpr uint32_t T28 = 0x455A14ED;
    static constexpr uint32_t T29 = 0xA9E3E905;
    static constexpr uint32_t T30 = 0xFCEFA3F8;
    static constexpr uint32_t T31 = 0x676F02D9;
    static constexpr uint32_t T32 = 0x8D2A4C8A;
    static constexpr uint32_t T33 = 0xFFFA3942;
    static constexpr uint32_t T34 = 0x8771F681;
    static constexpr uint32_t T35 = 0x6D9D6122;
    static constexpr uint32_t T36 = 0xFDE5380C;
    static constexpr uint32_t T37 = 0xA4BEEA44;
    static constexpr uint32_t T38 = 0x4BDECFA9;
    static constexpr uint32_t T39 = 0xF6BB4B60;
    static constexpr uint32_t T40 = 0xBEBFBC70;
    static constexpr uint32_t T41 = 0x289B7EC6;
    static constexpr uint32_t T42 = 0xEAA127FA;
    static constexpr uint32_t T43 = 0xD4EF3085;
    static constexpr uint32_t T44 = 0x04881D05;
    static constexpr uint32_t T45 = 0xD9D4D039;
    static constexpr uint32_t T46 = 0xE6DB99E5;
    static constexpr uint32_t T47 = 0x1FA27CF8;
    static constexpr uint32_t T48 = 0xC4AC5665;
    static constexpr uint32_t T49 = 0xF4292244;
    static constexpr uint32_t T50 = 0x432AFF97;
    static constexpr uint32_t T51 = 0xAB9423A7;
    static constexpr uint32_t T52 = 0xFC93A039;
    static constexpr uint32_t T53 = 0x655B59C3;
    static constexpr uint32_t T54 = 0x8F0CCC92;
    static constexpr uint32_t T55 = 0xFFEFF47D;
    static constexpr uint32_t T56 = 0x85845DD1;
    static constexpr uint32_t T57 = 0x6FA87E4F;
    static constexpr uint32_t T58 = 0xFE2CE6E0;
    static constexpr uint32_t T59 = 0xA3014314;
    static constexpr uint32_t T60 = 0x4E0811A1;
    static constexpr uint32_t T61 = 0xF7537E82;
    static constexpr uint32_t T62 = 0xBD3AF235;
    static constexpr uint32_t T63 = 0x2AD7D2BB;
    static constexpr uint32_t T64 = 0xEB86D391;

    inline static uint32_t rol32(uint32_t x, int r) {
        return (x << r) | (x >> (32 - r));
    }

    #ifndef swap32
    inline static uint32_t swap32(uint32_t z) {
        return (z << 24) | ((z & 0xFF00) << 8) | ((z & 0xFF0000) >> 8) | (z >> 24);
    }
    #endif

    inline static uint32_t f(uint32_t x, uint32_t y, uint32_t z) {
        return (x & y) | ((~x) & z);
    }
    inline static uint32_t g(uint32_t x, uint32_t y, uint32_t z) {
        return (x & z) | (y & ~z);
    }
    inline static uint32_t h(uint32_t x, uint32_t y, uint32_t z) {
        return x ^ y ^ z;
    }
    inline static uint32_t l(uint32_t x, uint32_t y, uint32_t z) {
        return y ^ (x | ~z);
    }

    inline static void r1(const uint32_t *x, uint32_t &a, uint32_t b, uint32_t c, uint32_t d, int k, int s, uint32_t t) {
        a = b + rol32(a + f(b, c, d) + x[k] + t, s);
    }
    inline static void r2(const uint32_t *x, uint32_t &a, uint32_t b, uint32_t c, uint32_t d, int k, int s, uint32_t t) {
        a = b + rol32(a + g(b, c, d) + x[k] + t, s);
    }
    inline static void r3(const uint32_t *x, uint32_t &a, uint32_t b, uint32_t c, uint32_t d, int k, int s, uint32_t t) {
        a = b + rol32(a + h(b, c, d) + x[k] + t, s);
    }
    inline static void r4(const uint32_t *x, uint32_t &a, uint32_t b, uint32_t c, uint32_t d, int k, int s, uint32_t t) {
        a = b + rol32(a + l(b, c, d) + x[k] + t, s);
    }

    Bytes aux_ { Bytes::allocate(AuxSize) };
    int auxFill_ { 0 };
    uint64_t bytesFeed_ { 0 };
    uint32_t a_ { 0x67452301 };
    uint32_t b_ { 0xEFCDAB89 };
    uint32_t c_ { 0x98BADCFE };
    uint32_t d_ { 0x10325476 };
};

Md5HashSink::Md5HashSink():
    CryptoHashSink{new State}
{}

Bytes md5(const Bytes &data)
{
    Md5HashSink hashSink;
    hashSink.write(data);
    return hashSink.finish();
}

} // namespace cc
