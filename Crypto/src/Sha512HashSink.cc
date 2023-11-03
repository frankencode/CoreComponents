/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Sha512HashSink>
#include <cc/bits>
#include <array>
#include <bit>

namespace cc {

struct Sha512HashSink::State final: public CryptoHashSink::State
{
    static constexpr int BlockSize = 128;

    State()
    {
        uint64_t * const H = static_cast<uint64_t *>(hash_);

        for (int i = 0; i < 8; ++i) {
            H[i] = iv_[i];
        }
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
            writeByte(data[i]);
        }
        l_ += fill;
    }

    void writeByte(uint8_t data)
    {
        feed_[j_++] = data;
        if (j_ == BlockSize) consume();
    }

    void consume()
    {
        j_ = 0;

        const uint64_t * const K = key_;
        const uint64_t * const M = static_cast<uint64_t *>(feed_);
        uint64_t * const W = static_cast<uint64_t *>(work_);
        uint64_t * const H = static_cast<uint64_t *>(hash_);

        for (int t = 0; t < 16; ++t) {
            if (std::endian::native == std::endian::big) {
                W[t] = M[t];
            }
            else {
                W[t] = byteSwap(M[t]);
            }
        }

        for (int t = 16; t < 80; ++t) {
            W[t] = lowerSigma1(W[t - 2]) + W[t - 7] + lowerSigma0(W[t - 15]) + W[t - 16];
        }

        uint64_t a = H[0];
        uint64_t b = H[1];
        uint64_t c = H[2];
        uint64_t d = H[3];
        uint64_t e = H[4];
        uint64_t f = H[5];
        uint64_t g = H[6];
        uint64_t h = H[7];

        for (int t = 0; t < 80; ++t)
        {
            const uint64_t T1 = h + upperSigma1(e) + ch(e, f, g) + K[t] + W[t];
            const uint64_t T2 = upperSigma0(a) + maj(a, b, c);
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

        writeByte(0x80);
        while (j_ % BlockSize != BlockSize - 16) writeByte(0);
        for (int i = 7; i >= 0; --i) writeByte(0);
        for (int i = 7; i >= 0; --i) writeByte((l_ >> (i * 8)) & 0xFF);

        if (std::endian::native != std::endian::big) {
            uint64_t * const H = static_cast<uint64_t *>(hash_);
            for (int t = 0; t < 8; ++t) {
                H[t] = byteSwap(H[t]);
            }
        }

        return hash_;
    }

    static uint64_t ch(uint64_t x, uint64_t y, uint64_t z)
    {
        return (x bitand y) xor (~x bitand z);
    }

    static uint64_t maj(uint64_t x, uint64_t y, uint64_t z)
    {
        return (x bitand y) xor (x bitand z) xor (y bitand z);
    }

    static uint64_t upperSigma0(uint64_t x)
    {
        return std::rotr(x, 28) xor std::rotr(x, 34) xor std::rotr(x, 39);
    }

    static uint64_t upperSigma1(uint64_t x)
    {
        return std::rotr(x, 14) xor std::rotr(x, 18) xor std::rotr(x, 41);
    }

    static uint64_t lowerSigma0(uint64_t x)
    {
        return std::rotr(x, 1) xor std::rotr(x, 8) xor (x >> 7);
    }

    static uint64_t lowerSigma1(uint64_t x)
    {
        return std::rotr(x, 19) xor std::rotr(x, 61) xor (x >> 6);
    }

    const uint64_t iv_[8] = {
        0x6a09e667f3bcc908,
        0xbb67ae8584caa73b,
        0x3c6ef372fe94f82b,
        0xa54ff53a5f1d36f1,
        0x510e527fade682d1,
        0x9b05688c2b3e6c1f,
        0x1f83d9abfb41bd6b,
        0x5be0cd19137e2179
    };

    const uint64_t key_[80] = {
        0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc,
        0x3956c25bf348b538, 0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118,
        0xd807aa98a3030242, 0x12835b0145706fbe, 0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
        0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235, 0xc19bf174cf692694,
        0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
        0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
        0x983e5152ee66dfab, 0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4,
        0xc6e00bf33da88fc2, 0xd5a79147930aa725, 0x06ca6351e003826f, 0x142929670a0e6e70,
        0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
        0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b,
        0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30,
        0xd192e819d6ef5218, 0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8,
        0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8,
        0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3,
        0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
        0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b,
        0xca273eceea26619c, 0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178,
        0x06f067aa72176fba, 0x0a637dc5a2c898a6, 0x113f9804bef90dae, 0x1b710b35131c471b,
        0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc, 0x431d67c49c100d4c,
        0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817
    };

    Bytes hash_ { Bytes::allocate(64) };
    Bytes feed_ { Bytes::allocate(128) };
    Bytes work_ { Bytes::allocate(640) };
    int j_ { 0 };
    int64_t l_ { 0 };
};

Sha512HashSink::Sha512HashSink():
    CryptoHashSink{new State}
{}

Bytes sha512(const Bytes &data)
{
    Sha512HashSink sink;
    sink.write(data);
    return sink.finish();
}

} // namespace cc
