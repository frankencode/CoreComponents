/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/PseudoPad>

namespace cc {

struct PseudoPad::State: public Stream::State
{
    State(const BlockCipher &cipher):
        cipher_{cipher},
        p_{Bytes::allocate(cipher.blockSize())},
        c_{Bytes::allocate(cipher.blockSize())},
        i_{cipher.blockSize()}
    {
        p_.fill(0);
    }

    long read(Out<Bytes> buffer, long maxFill) override
    {
        if (maxFill < 0) maxFill = buffer->count();
        if (maxFill == 0) return 0;

        for (long j = 0; j < maxFill; ++j) {
            if (i_ == c_.count()) {
                cipher_.encode(p_, &c_);
                increment(&p_);
                i_ = 0;
            }
            buffer->byteAt(j) = c_.byteAt(i_);
            ++i_;
        }

        return maxFill;
    }

    static void increment(InOut<Bytes> bigWord)
    {
        for (int i = 0, u = 1; i < bigWord->count() && u > 0; ++i) {
            int d = bigWord->byteAt(i);
            d = d + u;
            if (d == 0x100) { d = 0; u = 1; }
            else u = 0;
            bigWord->byteAt(i) = d;
        }
    }

    BlockCipher cipher_;
    Bytes p_;
    Bytes c_;
    int i_ { 0 };
};

PseudoPad::PseudoPad(const BlockCipher &cipher):
    Stream{new State{cipher}}
{}

} // namespace cc
