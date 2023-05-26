/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/buckets/BucketIndexTree>

namespace cc::buckets {

template class BucketIndexTree<16>;

template void BucketIndexTree<16>::dissipateSlow<BucketIndexTree<16>::Branch>(Branch *&node, unsigned &egress);
template void BucketIndexTree<16>::relieve<BucketIndexTree<16>::Branch>(Branch *node);
template void BucketIndexTree<16>::collapseSucc<BucketIndexTree<16>::Branch>(Branch *node, Branch *succ);

} // namespace cc::buckets
