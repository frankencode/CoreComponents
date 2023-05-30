/*
 * Copyright (C) 2020-2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/buckets/BucketTree>

namespace cc {

template class BucketTree<DefaultTreeGranularity>;

template void BucketTree<DefaultTreeGranularity>::dissipateSlow<BucketTree<DefaultTreeGranularity>::Branch>(Branch *&node, unsigned &egress);
template void BucketTree<DefaultTreeGranularity>::relieve<BucketTree<DefaultTreeGranularity>::Branch>(Branch *node);
template void BucketTree<DefaultTreeGranularity>::collapseSucc<BucketTree<DefaultTreeGranularity>::Branch>(Branch *node, Branch *succ);

} // namespace cc
