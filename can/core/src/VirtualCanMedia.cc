/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/VirtualCanMedia>
#include <cc/Channel>

namespace cc {
namespace can {

Ref<VirtualCanMedia> VirtualCanMedia::create(VirtualCanBus *bus)
{
    return new VirtualCanMedia{bus};
}

VirtualCanMedia::VirtualCanMedia(VirtualCanBus *bus):
    bus_{bus}
{}

void VirtualCanMedia::writeFrame(const CanFrame *frame)
{
    bus_->broadcast(this, frame);
}

}} // namespace cc::can
