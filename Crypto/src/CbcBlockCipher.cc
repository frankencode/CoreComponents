/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/CbcBlockCipher>

namespace cc {

struct CbcBlockCipher::State: public BlockCipher::State
{
    State(const BlockCipher &cipher, const Bytes &start):
        BlockCipher::State{cipher.blockSize()},
        cipher_{cipher},
        s_{Bytes::allocate(cipher.blockSize())}
    {
        s_.fill(0);
        if (start) start.copyTo(&s_);
    }

    void encode(const Bytes &p, Out<Bytes> c) override
    {
        p.applyTo<Xor>(&s_);
        cipher_.encode(s_, &c);
        c->copyTo(&s_);
    }

    void decode(const Bytes &c, Out<Bytes> p) override
    {
        cipher_.decode(c, &p);
        s_.applyTo<Xor>(&p);
        c.copyTo(&s_);
    }

    BlockCipher cipher_;
    Bytes s_;
};

CbcBlockCipher::CbcBlockCipher(const BlockCipher &cipher, const Bytes &start):
    BlockCipher{new State{cipher, start}}
{}

} // namespace cc
