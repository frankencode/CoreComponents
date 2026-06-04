/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/syntax_node/CharCompareNode>

namespace cc::syntax {

template class CharCompareNode<Equal>;
template class CharCompareNode<Differ>;

} // namespace cc::syntax
