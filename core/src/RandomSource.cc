/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Random>
#include <cc/RandomSource>

namespace cc {

Ref<RandomSource> RandomSource::open(int seed, off_t eoi, int byteMin, int byteMax)
{
    return new RandomSource(seed, eoi, byteMin, byteMax);
}

RandomSource::RandomSource(int seed, off_t eoi, int byteMin, int byteMax):
    random_(Random::open(seed)),
    byteMin_(byteMin),
    byteMax_(byteMax),
    n_(0),
    eoi_(eoi)
{}

int RandomSource::read(CharArray *data)
{
    if (n_ == eoi_) return 0;
    for (int i = 0; i < data->count(); ++i) {
        data->at(i) = random_->get(byteMin_, byteMax_);
        if (++n_ == eoi_) return i + 1;
    }
    return data->count();
}

} // namespace cc
