/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/VirtualCanBus>
#include <cc/can/CanFeed>
#include <cc/Guard>
#include <cc/Channel> /// \todo whatfore?

namespace cc {
namespace can {

class VirtualCanMedia;

class VirtualCanMediaInstance: public CanFeed::Instance
{
public:
    void writeFrame(const CanFrame &frame) override
    {
        FeedGuard guard{this};
        bus_->broadcast(frame);
    }

private:
    friend class VirtualCanMedia;

    VirtualCanMediaInstance(VirtualCanBus::Instance *bus):
        bus_{bus}
    {}

    Ref<VirtualCanBus::Instance> bus_;
};

class VirtualCanMedia: public CanFeed
{
public:
    VirtualCanMedia(VirtualCanBus::Instance *bus):
        CanFeed{new VirtualCanMediaInstance{bus}}
    {}
};

CanMedia VirtualCanBus::Instance::connect()
{
    VirtualCanMedia media{this};
    {
        Guard<Mutex> guard{mutex_};
        connections_->append(media);
    }
    return media;
}

void VirtualCanBus::Instance::shutdown()
{
    Guard<Mutex> guard{mutex_};
    for (CanFeed &media: connections_)
        media->shutdown();

    connections_->deplete();
}

void VirtualCanBus::Instance::broadcast(const CanFrame &frame)
{
    Guard<Mutex> guard{mutex_};
    for (CanFeed &media: connections_)
        media->feedFrame(frame);
}

}} // namespace cc::can
