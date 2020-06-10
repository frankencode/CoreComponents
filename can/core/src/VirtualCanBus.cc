/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/VirtualCanBus>
#include <cc/can/VirtualCanMedia>
#include <cc/Guard>
#include <cc/Channel>

namespace cc {
namespace can {

Ref<VirtualCanBus> VirtualCanBus::create()
{
    return new VirtualCanBus;
}

VirtualCanBus::VirtualCanBus()
{}

Ref<CanMedia> VirtualCanBus::connect()
{
    auto media = VirtualCanMedia::create(this);
    {
        Guard<Mutex> guard{mutex_};
        connections_->insert(media);
    }
    return media;
}

void VirtualCanBus::shutdown()
{
    Guard<Mutex> guard{mutex_};
    for (VirtualCanMedia *media: connections_)
        media->shutdown();
    connections_->deplete();
}

void VirtualCanBus::broadcast(const VirtualCanMedia *source, const CanFrame &frame)
{
    Guard<Mutex> guard{mutex_};
    for (VirtualCanMedia *media: connections_) {
        if (media != source)
            media->feedFrame(frame);
    }
}

}} // namespace cc::can
