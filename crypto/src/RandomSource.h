/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Stream>

namespace cc {
namespace crypto {

/** \class RandomSource RandomSource.h cc/crypto/RandomSource
  * \brief Pseudo random number source
  */
class RandomSource: public Stream
{
public:
    static Ref<RandomSource> open(const ByteArray *salt = 0);

    virtual int read(ByteArray *data);

private:
    RandomSource(const ByteArray *salt);
    Ref<Stream> source_;
};

}} // namespace cc::crypto
