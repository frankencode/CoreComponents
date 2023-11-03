/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Sha256HashSink>
#include <array>
#include <bit>

namespace cc {

struct Sha256HashSink::State: public CryptoHashSink::State
{
    static constexpr int BlockSize = 64;

    State()
    {
        hash_.wordAt(0) = 0x6a09e667;
        hash_.wordAt(1) = 0xbb67ae85;
        hash_.wordAt(2) = 0x3c6ef372;
        hash_.wordAt(3) = 0xa54ff53a;
        hash_.wordAt(4) = 0x510e527f;
        hash_.wordAt(5) = 0x9b05688c;
        hash_.wordAt(6) = 0x1f83d9ab;
        hash_.wordAt(7) = 0x5be0cd19;
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
            block_.byteAt(j_++) = data.byteAt(i);
            if (j_ == 64) consume();
        }
        l_ += fill;
    }

    void consume()
    {
        j_ = 0;

        const auto &K = key_;
        uint32_t * const W = sched_.words();
        uint32_t * const H = hash_.words();

        for (int t = 0; t < 16; ++t) {
            W[t] =
                uint32_t(block_.byteAt(4 * t) << 24) |
                uint32_t(block_.byteAt(4 * t + 1) << 16) |
                uint32_t(block_.byteAt(4 * t + 2) << 8) |
                uint32_t(block_.byteAt(4 * t + 3));
        }

        for (int t = 16; t < 64; ++t) {
            W[t] = lowerSigma1(W[t - 2]) + W[t - 7] + lowerSigma0(W[t - 15]) + W[t - 16];
        }

        uint32_t a = H[0];
        uint32_t b = H[1];
        uint32_t c = H[2];
        uint32_t d = H[3];
        uint32_t e = H[4];
        uint32_t f = H[5];
        uint32_t g = H[6];
        uint32_t h = H[7];

        for (int t = 0; t < 64; ++t)
        {
            const uint32_t T1 = h + upperSigma1(e) + ch(e, f, g) + K[t] + W[t];
            const uint32_t T2 = upperSigma0(a) + maj(a, b, c);
            h = g;
            g = f;
            f = e;
            e = d + T1;
            d = c;
            c = b;
            b = a;
            a = T1 + T2;
        }

        H[0] += a;
        H[1] += b;
        H[2] += c;
        H[3] += d;
        H[4] += e;
        H[5] += f;
        H[6] += g;
        H[7] += h;
    }

    Bytes finish() override
    {
        l_ *= 8;

        block_.byteAt(j_++) = 0x80;
        if (j_ == 64) consume();
        while (j_ % 64 != 56) {
            block_.byteAt(j_++) = 0;
            if (j_ == 64) consume();
        }
        for (int i = 7; i >= 0; --i) {
            block_.byteAt(j_++) = (l_ >> (i * 8)) & 0xFF;
        }
        consume();

        if (std::endian::native != std::endian::big)
        {
            for (int t = 0; t < 8; ++t) {
                uint32_t h = hash_.wordAt(t);
                hash_[4 * t] = h >> 24;
                hash_[4 * t + 1] = (h >> 16) & 0xFF;
                hash_[4 * t + 2] = (h >> 8) & 0xFF;
                hash_[4 * t + 3] = h & 0xFF;
            }
        }

        return hash_;
    }

    static uint32_t ch(uint32_t x, uint32_t y, uint32_t z)
    {
        return (x bitand y) xor (~x bitand z);
    }

    static uint32_t maj(uint32_t x, uint32_t y, uint32_t z)
    {
        return (x bitand y) xor (x bitand z) xor (y bitand z);
    }

    static uint32_t upperSigma0(uint32_t x)
    {
        return std::rotr(x, 2) xor std::rotr(x, 13) xor std::rotr(x, 22);
    }

    static uint32_t upperSigma1(uint32_t x)
    {
        return std::rotr(x, 6) xor std::rotr(x, 11) xor std::rotr(x, 25);
    }

    static uint32_t lowerSigma0(uint32_t x)
    {
        return std::rotr(x, 7) xor std::rotr(x, 18) xor (x >> 3);
    }

    static uint32_t lowerSigma1(uint32_t x)
    {
        return std::rotr(x, 17) xor std::rotr(x, 19) xor (x >> 10);
    }

    std::array<uint32_t, 64> key_ {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };

    Bytes hash_ { Bytes::allocate(32) };
    Bytes block_ { Bytes::allocate(64) };
    Bytes sched_ { Bytes::allocate(256) };
    int j_ { 0 };
    int64_t l_ { 0 };
};

Sha256HashSink::Sha256HashSink():
    CryptoHashSink{new State}
{}

Bytes sha256(const Bytes &data)
{
    Sha256HashSink sink;
    sink.write(data);
    return sink.finish();
}

} // namespace cc
