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

class Random;

/** \class RandomSource RandomSource.h cc/RandomSource
  * \brief %Random data source
  * \see Random
  */
class RandomSource: public Stream
{
public:
    static Ref<RandomSource> open() { return new RandomSource; }
    virtual int read(ByteArray *data) override;

private:
    RandomSource();

    Ref<Random> random_;
};

} // namespace cc
