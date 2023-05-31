/*
 * Copyright (C) 2020-2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/buckets/BucketTree>

namespace cc {

template class BucketTree<BucketTreeGranularity>;

template void BucketTree<BucketTreeGranularity>::dissipateSlow<BucketTree<BucketTreeGranularity>::Branch>(Branch *&node, unsigned &egress);
template void BucketTree<BucketTreeGranularity>::relieve<BucketTree<BucketTreeGranularity>::Branch>(Branch *node);
template void BucketTree<BucketTreeGranularity>::collapseSucc<BucketTree<BucketTreeGranularity>::Branch>(Branch *node, Branch *succ);

} // namespace cc
