/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Random>
#include <cc/RandomSource>

namespace cc {

RandomSource::RandomSource():
    random_(Random::open())
{}

int RandomSource::read(ByteArray *data)
{
    for (int i = 0; i < data->count(); ++i)
        data->at(i) = random_->get(0, 0xFF);
    return data->count();
}

} // namespace cc
