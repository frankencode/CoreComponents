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

RandomSource::RandomSource(int seed):
    random_(Random::open(seed))
{}

int RandomSource::read(CharArray *data)
{
    for (int i = 0; i < data->count(); ++i)
        data->at(i) = random_->get(0, 0xFF);
    return data->count();
}

} // namespace cc
