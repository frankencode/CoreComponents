/*
 * Copyright (C) 2020-2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/blist/Tree>

namespace cc::blist {

template class Tree<Granularity>;

template void Tree<Granularity>::dissipateSlow<Tree<Granularity>::Branch>(Branch *&node, unsigned &egress);
template void Tree<Granularity>::relieve<Tree<Granularity>::Branch>(Branch *node);
template void Tree<Granularity>::collapseSucc<Tree<Granularity>::Branch>(Branch *node, Branch *succ);

} // namespace cc::blist
