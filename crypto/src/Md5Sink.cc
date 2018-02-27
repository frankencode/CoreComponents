/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/crypto/Md5Sink>

namespace cc {
namespace crypto {

Ref<Md5Sink> Md5Sink::open()
{
    return new Md5Sink;
}

Md5Sink::Md5Sink():
    aux_(String::create(0x4000 + 64)),
    auxFill_(0),
    bytesFeed_(0),
    a_(0x67452301), b_(0xEFCDAB89),
    c_(0x98BADCFE), d_(0x10325476)
{
    CC_ASSERT((aux_->count() % 64) == 0);
}

void Md5Sink::write(const CharArray *data)
{
    const uint8_t *src = data->bytes();
    int srcLeft = data->count();
    while (srcLeft > 0) {
        while ((auxFill_ < aux_->count()) && (srcLeft > 0)) {
            int n = aux_->count() - auxFill_;
            if (n > srcLeft) n = srcLeft;
            memcpy(mutate(aux_)->bytes() + auxFill_, src, n);
            src += n;
            srcLeft -= n;
            auxFill_ += n;
            bytesFeed_ += n;
        }
        if (auxFill_ % 64 == 0)
            consume();
    }
}

String Md5Sink::finish()
{
    /** manually feed the padding and message size
      */
    mutate(aux_)->byteAt(auxFill_++) = 0x80;
    while ((auxFill_ % 64) != 56)
        mutate(aux_)->byteAt(auxFill_++) = 0x00;
    uint64_t b = bytesFeed_ << 3; // message length in bits
    for (int i = 0; i < 8; ++i)
        mutate(aux_)->byteAt(auxFill_++) = b >> (i * 8);
    consume();

    /** serialize the message digest
      */
    String h = String::create(Md5Sink::Size);
    int k = 0;
    for (int i = 0; i < 4; ++i) mutate(h)->at(k++) = a_ >> (8 * i);
    for (int i = 0; i < 4; ++i) mutate(h)->at(k++) = b_ >> (8 * i);
    for (int i = 0; i < 4; ++i) mutate(h)->at(k++) = c_ >> (8 * i);
    for (int i = 0; i < 4; ++i) mutate(h)->at(k++) = d_ >> (8 * i);
    return h;
}

/** Ti = uint32_t(4294967296. * fabs(sin(i))) for i = 1..64
  */
#define T1 0xD76AA478
#define T2 0xE8C7B756
#define T3 0x242070DB
#define T4 0xC1BDCEEE
#define T5 0xF57C0FAF
#define T6 0x4787C62A
#define T7 0xA8304613
#define T8 0xFD469501
#define T9 0x698098D8
#define T10 0x8B44F7AF
#define T11 0xFFFF5BB1
#define T12 0x895CD7BE
#define T13 0x6B901122
#define T14 0xFD987193
#define T15 0xA679438E
#define T16 0x49B40821
#define T17 0xF61E2562
#define T18 0xC040B340
#define T19 0x265E5A51
#define T20 0xE9B6C7AA
#define T21 0xD62F105D
#define T22 0x02441453
#define T23 0xD8A1E681
#define T24 0xE7D3FBC8
#define T25 0x21E1CDE6
#define T26 0xC33707D6
#define T27 0xF4D50D87
#define T28 0x455A14ED
#define T29 0xA9E3E905
#define T30 0xFCEFA3F8
#define T31 0x676F02D9
#define T32 0x8D2A4C8A
#define T33 0xFFFA3942
#define T34 0x8771F681
#define T35 0x6D9D6122
#define T36 0xFDE5380C
#define T37 0xA4BEEA44
#define T38 0x4BDECFA9
#define T39 0xF6BB4B60
#define T40 0xBEBFBC70
#define T41 0x289B7EC6
#define T42 0xEAA127FA
#define T43 0xD4EF3085
#define T44 0x04881D05
#define T45 0xD9D4D039
#define T46 0xE6DB99E5
#define T47 0x1FA27CF8
#define T48 0xC4AC5665
#define T49 0xF4292244
#define T50 0x432AFF97
#define T51 0xAB9423A7
#define T52 0xFC93A039
#define T53 0x655B59C3
#define T54 0x8F0CCC92
#define T55 0xFFEFF47D
#define T56 0x85845DD1
#define T57 0x6FA87E4F
#define T58 0xFE2CE6E0
#define T59 0xA3014314
#define T60 0x4E0811A1
#define T61 0xF7537E82
#define T62 0xBD3AF235
#define T63 0x2AD7D2BB
#define T64 0xEB86D391

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

void Md5Sink::consume()
{
    uint32_t *m = mutate(aux_)->words();
    for (int i = 0, nc = auxFill_ / 64; i < nc; ++i) {
        // select 16 dwords x[0], x[1] ... x[15] and
        // ensure value of the dwords is independent from system endianess
        uint32_t *x = m + i * 16;
        if (localEndian() == BigEndian) {
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

String md5(const CharArray *data)
{
    Ref<Md5Sink> h = Md5Sink::open();
    h->write(data);
    return h->finish();
}

}} // namespace cc::crypto
