/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/BlockCipherSource>

namespace cc {

struct BlockCipherSource::State: public Stream::State
{
    State(const Stream &source, const BlockCipher &cipher):
        source_{source},
        cipher_{cipher},
        block_{Bytes::allocate(cipher.blockSize())}
    {}

    ~State()
    {
        block_.fill(0);
        decodeBuffer_.select(0, u_).fill(0);
    }

    long read(Out<Bytes> outBuffer, long maxFill = -1)
    {
        if (m_ < 0) return 0;
        if (maxFill < 0) maxFill = outBuffer->count();
        if (maxFill == 0) return 0;

        if (j_ == m_)
        {
            // refill decoding buffer

            j_ = m_ = 0;

            if (pending_) {
                pending_.copyTo(&decodeBuffer_);
                m_ = pending_.count();
                pending_.fill(0);
                pending_ = Bytes{};
            }

            const int n_b = cipher_.blockSize();

            while (m_ < n_b) {
                Bytes dst = decodeBuffer_.select(m_, decodeBuffer_.count() - m_);
                int n_r = source_.read(&dst);
                if (n_r == 0) {
                    m_ = -1;
                    return 0;
                }
                m_ += n_r;
            }

            if (m_ > u_) u_ = m_;

            // cut down to a multiple of the block size

            const int o = m_ % n_b;
            if (o != 0) {
                pending_ = decodeBuffer_.copy(m_ - o, m_);
                m_ -= o;
            }

            // decode contents of decoding buffer

            for (long i = 0; i + n_b <= m_; i += n_b) {
                Bytes s = decodeBuffer_.select(i, i + n_b);
                cipher_.decode(s, &block_);
                block_.copyTo(&s);
            }
        }

        const int availCount = m_ - j_;
        const int consumeCount = maxFill < availCount ? static_cast<int>(maxFill) : availCount;
        decodeBuffer_.copyRangeTo(j_, j_ + consumeCount, &outBuffer);
        j_ += consumeCount;
        return consumeCount;
    }

    Stream source_;
    BlockCipher cipher_;
    Bytes block_; // a single decode block
    Bytes decodeBuffer_ { Bytes::allocate(0x1000) }; // in-situ decoding buffer
    Bytes pending_; // overhang which did not fit into a multiple of a block size
    int j_ { 0 }; // consume offset in decode buffer
    int m_ { 0 }; // fill count of decode buffer
    int u_ { 0 }; // max fill count of decode buffer
};

BlockCipherSource::BlockCipherSource(const Stream &source, const BlockCipher &cipher):
    Stream{new State{source, cipher}}
{}

} // namespace cc
