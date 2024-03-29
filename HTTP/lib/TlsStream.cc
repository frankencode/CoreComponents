/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TlsStreamState>

namespace cc {

TlsStream::TlsStream(State *newState):
    Stream{newState}
{}

} // namespace cc
