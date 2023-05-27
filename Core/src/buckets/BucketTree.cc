/*
 * Copyright (C) 2020-2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/buckets/BucketTree>

namespace cc {

template class BucketTree<DefaultGranularity>;

template void BucketTree<DefaultGranularity>::dissipateSlow<BucketTree<DefaultGranularity>::Branch>(Branch *&node, unsigned &egress);
template void BucketTree<DefaultGranularity>::relieve<BucketTree<DefaultGranularity>::Branch>(Branch *node);
template void BucketTree<DefaultGranularity>::collapseSucc<BucketTree<DefaultGranularity>::Branch>(Branch *node, Branch *succ);

} // namespace cc
