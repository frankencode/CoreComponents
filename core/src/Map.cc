/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Variant>
#include <cc/Map>

namespace cc {

template class Map<String, String>;
template class Map<String, Variant>;

} // namespace cc
