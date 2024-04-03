/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax_node/CharCompareNode>

namespace cc::syntax {

template class CharCompareNode<Equal>;
template class CharCompareNode<Differ>;

} // namespace cc::syntax
