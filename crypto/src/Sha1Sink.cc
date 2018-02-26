/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/crypto/Sha1Sink>

namespace cc {
namespace crypto {

namespace _sha1 {

inline uint32_t rol(uint32_t x, int n)
{
    return (x << n) | (x >> (32 - n));
}

inline uint32_t K(int t)
{
    if (t < 20) return 0x5A827999;
    if (t < 40) return 0x6ED9EBA1;
    if (t < 60) return 0x8F1BBCDC;
    return 0xCA62C1D6;
}

inline uint32_t f(int t, uint32_t b, uint32_t c, uint32_t d)
{
    if (t < 20) return (b & c) | ((~b) & d);
    if (t < 40) return b ^ c ^ d;
    if (t < 60) return (b & c) | (b & d) | (c & d);
    return b ^ c ^ d;
}

} // namespace _sha1

Ref<Sha1Sink> Sha1Sink::open()
{
    return new Sha1Sink;
}

Sha1Sink::Sha1Sink():
    h_(String::create(20)),
    m_(String::create(64)),
    w_(String::create(320)),
    j_(0),
    l_(0)
{
    mutate(h_)->wordAt(0) = 0x67452301;
    mutate(h_)->wordAt(1) = 0xEFCDAB89;
    mutate(h_)->wordAt(2) = 0x98BADCFE;
    mutate(h_)->wordAt(3) = 0x10325476;
    mutate(h_)->wordAt(4) = 0xC3D2E1F0;
}

void Sha1Sink::write(const ByteArray *data)
{
    for (int i = 0; i < data->count(); ++i) {
        uint8_t b = data->byteAt(i);
        mutate(m_)->byteAt(j_++) = b;
        if (j_ == 64) consume();
    }
    l_ += uint64_t(data->count()) * 8;
}

String Sha1Sink::finish()
{
    mutate(m_)->byteAt(j_++) = 0x80;
    if (j_ == 64) consume();
    while (j_ % 64 != 56) {
        mutate(m_)->byteAt(j_++) = 0;
        if (j_ == 64) consume();
    }
    for (int i = 7; i >= 0; --i)
        mutate(m_)->byteAt(j_++) = (l_ >> (i * 8)) & 0xFF;

    consume();

    for (int t = 0; t < 5; ++t) {
        uint32_t h = h_->wordAt(t);
        mutate(h_)->byteAt(4 * t) = h >> 24;
        mutate(h_)->byteAt(4 * t + 1) = (h >> 16) & 0xFF;
        mutate(h_)->byteAt(4 * t + 2) = (h >> 8) & 0xFF;
        mutate(h_)->byteAt(4 * t + 3) = h & 0xFF;
    }

    return h_;
}

void Sha1Sink::consume()
{
    j_ = 0;

    uint32_t *w = &mutate(w_)->wordAt(0);

    for (int t = 0; t < 16; ++t) {
        w[t] =
            uint32_t(m_->byteAt(4 * t) << 24) |
            uint32_t(m_->byteAt(4 * t + 1) << 16) |
            uint32_t(m_->byteAt(4 * t + 2) << 8) |
            uint32_t(m_->byteAt(4 * t + 3));
    }

    for (int t = 16; t < 80; ++t)
        w[t] = _sha1::rol(w[t - 3] ^ w[t - 8] ^ w[t - 14] ^ w[t - 16], 1);

    uint32_t *h = &mutate(h_)->wordAt(0);

    uint32_t a = h[0], b = h[1], c = h[2], d = h[3], e = h[4];

    for (int t = 0; t < 80; ++t) {
        uint32_t temp = _sha1::rol(a, 5) + _sha1::f(t, b, c, d) + e + w[t] + _sha1::K(t);
        e = d;
        d = c;
        c = _sha1::rol(b, 30);
        b = a;
        a = temp;
    }

    h[0] += a;
    h[1] += b;
    h[2] += c;
    h[3] += d;
    h[4] += e;
}

String sha1(const ByteArray *data)
{
    Ref<Sha1Sink> h = Sha1Sink::open();
    h->write(data);
    return h->finish();
}

}} // namespace cc::crypto
