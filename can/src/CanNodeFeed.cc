/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/CanNodeFeed>

namespace cc {
namespace can {

Ref<CanNodeFeed> CanNodeFeed::create(CanMedia *sink)
{
    return new CanNodeFeed{sink};
}

CanNodeFeed::CanNodeFeed(CanMedia *sink):
    sink_{sink}
{}

void CanNodeFeed::writeFrame(const CanFrame *frame)
{
    sink_->writeFrame(frame);
}

}} // namespace cc::can
