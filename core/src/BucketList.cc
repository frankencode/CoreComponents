/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/String>
#include <cc/Variant>
#include <cc/BucketList>

namespace cc {

template class BucketList<String>;
template class BucketList<Variant>;

} // namespace cc
