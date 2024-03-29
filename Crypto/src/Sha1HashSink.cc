/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Sha1HashSink>
#include <bit>

namespace cc {

struct Sha1HashSink::State final: public CryptoHashSink::State
{
    static constexpr int BlockSize = 64;

    State()
    {
        h_.wordAt(0) = 0x67452301;
        h_.wordAt(1) = 0xEFCDAB89;
        h_.wordAt(2) = 0x98BADCFE;
        h_.wordAt(3) = 0x10325476;
        h_.wordAt(4) = 0xC3D2E1F0;
    }

    int blockSize() const override { return BlockSize; }

    void write(const List<Bytes> &buffers) override
    {
        CryptoHashSink::State::write(buffers);
    }

    void write(const struct iovec *iov, int iovcnt) override
    {
        CryptoHashSink::State::write(iov, iovcnt);
    }

    void write(const Bytes &data, long fill = -1) override
    {
        if (fill < 0) fill = data.count();
        for (long i = 0; i < fill; ++i) {
            uint8_t b = data.byteAt(i);
            m_.byteAt(j_++) = b;
            if (j_ == 64) consume();
        }
        l_ += uint64_t(fill) * 8;
    }

    Bytes finish() override
    {
        m_.byteAt(j_++) = 0x80;
        if (j_ == 64) consume();
        while (j_ % 64 != 56) {
            m_.byteAt(j_++) = 0;
            if (j_ == 64) consume();
        }
        for (int i = 7; i >= 0; --i) {
            m_.byteAt(j_++) = (l_ >> (i * 8)) & 0xFF;
        }
        consume();

        if (std::endian::native != std::endian::big)
        {
            for (int t = 0; t < 5; ++t) {
                uint32_t h = h_.wordAt(t);
                h_[4 * t] = h >> 24;
                h_[4 * t + 1] = (h >> 16) & 0xFF;
                h_[4 * t + 2] = (h >> 8) & 0xFF;
                h_[4 * t + 3] = h & 0xFF;
            }
        }

        return h_;
    }

    void consume()
    {
        j_ = 0;

        uint32_t *w = w_.words();

        for (int t = 0; t < 16; ++t) {
            w[t] =
                uint32_t(m_.byteAt(4 * t) << 24) |
                uint32_t(m_.byteAt(4 * t + 1) << 16) |
                uint32_t(m_.byteAt(4 * t + 2) << 8) |
                uint32_t(m_.byteAt(4 * t + 3));
        }

        for (int t = 16; t < 80; ++t) {
            w[t] = std::rotl(w[t - 3] ^ w[t - 8] ^ w[t - 14] ^ w[t - 16], 1);
        }

        uint32_t *h = h_.words();

        uint32_t a = h[0], b = h[1], c = h[2], d = h[3], e = h[4];

        for (int t = 0; t < 80; ++t) {
            uint32_t temp = std::rotl(a, 5) + f(t, b, c, d) + e + w[t] + K(t);
            e = d;
            d = c;
            c = std::rotl(b, 30);
            b = a;
            a = temp;
        }

        h[0] += a;
        h[1] += b;
        h[2] += c;
        h[3] += d;
        h[4] += e;
    }

    static uint32_t K(int t)
    {
        if (t < 20) return 0x5A827999;
        if (t < 40) return 0x6ED9EBA1;
        if (t < 60) return 0x8F1BBCDC;
        return 0xCA62C1D6;
    }

    static uint32_t f(int t, uint32_t b, uint32_t c, uint32_t d)
    {
        if (t < 20) return (b & c) | ((~b) & d);
        if (t < 40) return b ^ c ^ d;
        if (t < 60) return (b & c) | (b & d) | (c & d);
        return b ^ c ^ d;
    }

    Bytes h_ { Bytes::allocate(20) };
    Bytes m_ { Bytes::allocate(64) };
    Bytes w_ { Bytes::allocate(320) };
    int j_ { 0 };
    uint64_t l_ { 0 };
};

Sha1HashSink::Sha1HashSink():
    CryptoHashSink{new State}
{}

Bytes sha1(const Bytes &data)
{
    Sha1HashSink sink;
    sink.write(data);
    return sink.finish();
}

} // namespace cc
