/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/TlsStreamState>

namespace cc {

TlsStream::TlsStream(State *newState):
    Stream{newState}
{}

} // namespace cc
