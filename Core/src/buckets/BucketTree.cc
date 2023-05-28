/*
 * Copyright (C) 2020-2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/buckets/BucketTree>

namespace cc {

template class BucketTree<DefaultBucketSize>;

template void BucketTree<DefaultBucketSize>::dissipateSlow<BucketTree<DefaultBucketSize>::Branch>(Branch *&node, unsigned &egress);
template void BucketTree<DefaultBucketSize>::relieve<BucketTree<DefaultBucketSize>::Branch>(Branch *node);
template void BucketTree<DefaultBucketSize>::collapseSucc<BucketTree<DefaultBucketSize>::Branch>(Branch *node, Branch *succ);

} // namespace cc
