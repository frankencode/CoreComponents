/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/String>
#include <cc/Variant>
#include <cc/NextList>

namespace cc {

template class NextList<String>;
template class NextList<Variant>;

} // namespace cc
