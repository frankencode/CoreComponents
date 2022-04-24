/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Entity>

namespace cc {

Entity::State::~State()
{
    for (auto &p: pointers_) {
        const_cast<VoidPointer &>(p)->target_ = nullptr;
    }
}

} // namespace cc
