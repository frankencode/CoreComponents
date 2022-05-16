/*
 * Copyright (C) 2019-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/VirtualCanBus>
#include <cc/CanFeed>
#include <cc/List>
#include <cc/Mutex>

namespace cc {

class VirtualCanMedia final: public CanFeed
{
public:
    explicit VirtualCanMedia(const VirtualCanBus &bus):
        CanFeed{new State{bus}}
    {}

private:
    struct State: public CanFeed::State
    {
        explicit State(const VirtualCanBus &bus):
            bus_{bus}
        {}

        void writeFrame(const CanFrame &frame) override;

        VirtualCanBus bus_;
    };
};

struct VirtualCanBus::State: public Object::State
{
    CanMedia connect()
    {
        VirtualCanMedia media{alias<VirtualCanBus>(this)};
        Guard<Mutex> guard{mutex_};
        connections_.append(media);
        return media;
    }

    void shutdown()
    {
        Guard<Mutex> guard{mutex_};
        for (CanFeed &media: connections_) {
            media.shutdown();
        }
        connections_.deplete();
    }

    void broadcast(const CanFrame &frame, const CanFeed &source)
    {
        Guard<Mutex> guard{mutex_};
        for (CanFeed &media: connections_) {
            if (source != media) {
                media.feedFrame(frame);
            }
        }
    }

    List<CanFeed> connections_;
    Mutex mutex_;
};

void VirtualCanMedia::State::writeFrame(const CanFrame &frame)
{
    bus_->broadcast(frame, alias<CanFeed>(this));
}

VirtualCanBus::VirtualCanBus():
    Object{new State}
{}

CanMedia VirtualCanBus::connect()
{
    return me().connect();
}

void VirtualCanBus::shutdown()
{
    me().shutdown();
}

VirtualCanBus::State &VirtualCanBus::me()
{
    return Object::me.as<State>();
}

const VirtualCanBus::State &VirtualCanBus::me() const
{
    return Object::me.as<State>();
}

VirtualCanBus::State *VirtualCanBus::operator->()
{
    return &me();
}

} // namespace cc
