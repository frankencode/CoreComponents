/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Stream>
#include <cc/crypto/BlockCipher>

namespace cc {
namespace crypto {

/** \class PseudoPad PseudoPad.h cc/crypto/PseudoPad
  * \brief pseudo random byte source
  *
  * The PseudoPad runs a given block cipher in counter mode to generate a random byte sequence.
  */
class PseudoPad: public Stream
{
public:
    static Ref<PseudoPad> create(BlockCipher *cipher);
    virtual int read(ByteArray *data);

private:
    PseudoPad(BlockCipher *cipher);
    static void increment(ByteArray *bigWord);

    Ref<BlockCipher> cipher_;
    Ref<ByteArray> p_;
    Ref<ByteArray> c_;
    int i_;
};

}} // namespace cc::crypto
