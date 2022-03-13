/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/syntax/CharCompareNode>

namespace cc::syntax {

template class CharCompareNode<Equal>;
template class CharCompareNode<Differ>;

} // namespace cc::syntax
