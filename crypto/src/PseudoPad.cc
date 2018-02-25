/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/crypto/PseudoPad>

namespace cc {
namespace crypto {

Ref<PseudoPad> PseudoPad::open(BlockCipher *cipher)
{
    return new PseudoPad(cipher);
}

PseudoPad::PseudoPad(BlockCipher *cipher):
    cipher_(cipher),
    p_(ByteArray::allocate(cipher->blockSize())),
    c_(ByteArray::allocate(cipher->blockSize())),
    i_(c_->count())
{
    mutate(p_)->fill(0);
}

void PseudoPad::increment(ByteArray *bigWord)
{
    for (int i = 0, u = 1; i < bigWord->count() && u > 0; ++i) {
        int d = bigWord->byteAt(i);
        d = d + u;
        if (d == 0x100) { d = 0; u = 1; }
        else u = 0;
        bigWord->byteAt(i) = d;
    }
}

int PseudoPad::read(ByteArray *data)
{
    for (int j = 0; j < data->count(); ++j) {
        if (i_ == c_->count()) {
            cipher_->encode(p_, mutate(c_));
            increment(mutate(p_));
            i_ = 0;
        }
        data->byteAt(j) = c_->byteAt(i_);
        ++i_;
    }
    return data->count();
}

}} // namespace cc::crypto
