/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/crypto/CipherSource>

namespace cc {
namespace crypto {

Ref<CipherSource> CipherSource::open(BlockCipher *cipher, Stream *source)
{
    return new CipherSource(cipher,  source);
}

CipherSource::CipherSource(BlockCipher *cipher, Stream *source):
    cipher_(cipher),
    source_(source),
    block_(String::allocate(cipher_->blockSize())),
    buffer_(String::allocate(0x1000)),
    j_(0), m_(0)
{}

int CipherSource::read(ByteArray *data)
{
    if (m_ < 0) return 0;

    const int n_b = cipher_->blockSize();

    if (m_ == 0)
    {
        // refill decoding buffer

        while (m_ < n_b) {
            if (pending_) {
                mutate(buffer_->select(m_, buffer_->count() - m_))->write(pending_);
                m_ += pending_->count();
                pending_ = 0;
            }
            else {
                int n_r = source_->read(mutate(buffer_->select(m_, buffer_->count() - m_)));
                if (n_r == 0) {
                    m_ = -1;
                    return 0;
                }
                m_ += n_r;
            }
        }

        const int o = m_ % n_b; // overhang

        if (o != 0) {
            pending_ = buffer_->copy(m_ - o, m_);
            m_ -= o;
        }

        // decode contents of decoding buffer

        for (int i = 0; i + n_b <= m_; i += n_b) {
            String s = buffer_->select(i, i + n_b);
            cipher_->decode(s, mutate(block_));
            mutate(s)->write(block_);
        }

        j_= 0;
    }

    String avail = buffer_->select(j_, m_);
    data->write(avail);
    if (data->count() < avail->count()) {
        j_ += data->count();
        return data->count();
    }

    m_ = 0;
    return avail->count();
}

}} // namespace cc::crypto
