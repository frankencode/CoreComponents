/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/TlsStreamState>

namespace cc {

TlsStream::TlsStream(State *newState):
    Stream{newState}
{}

} // namespace cc
