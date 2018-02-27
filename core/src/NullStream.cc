/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Singleton>
#include <cc/NullStream>

namespace cc {

NullStream *NullStream::instance()
{
    return Singleton<NullStream>::instance();
}

int NullStream::read(CharArray *data)
{
    data->fill(0);
    return data->count();
}

void NullStream::write(const CharArray *data)
{}

} // namespace cc
