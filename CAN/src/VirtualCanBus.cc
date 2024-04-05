/*
 * Copyright (C) 2019-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/VirtualCanBus>
#include <cc/CanFrameFeed>
#include <cc/List>
#include <cc/Mutex>

namespace cc {

class CanVirtualFeed final: public CanFrameFeed
{
public:
    CanVirtualFeed() = default;

    explicit CanVirtualFeed(const VirtualCanBus &bus):
        CanFrameFeed{new State{bus}}
    {}

private:
    struct State: public CanFrameFeed::State
    {
        explicit State(const VirtualCanBus &bus):
            bus_{bus}
        {}

        void write(const CanFrame &frame) override;

        VirtualCanBus bus_;
    };
};

struct VirtualCanBus::State: public Object::State
{
    CanMedia connect()
    {
        CanVirtualFeed feed{alias<VirtualCanBus>(this)};
        Guard<Mutex> guard{mutex_};
        connections_.append(feed);
        return feed;
    }

    void shutdown()
    {
        Guard<Mutex> guard{mutex_};
        for (CanVirtualFeed &feed: connections_) {
            feed.shutdown();
        }
        connections_.deplete();
    }

    void broadcast(const CanFrame &frame, const CanFrameFeed &source)
    {
        Guard<Mutex> guard{mutex_};
        for (CanVirtualFeed &feed: connections_) {
            if (source != feed) {
                feed.feedFrame(frame);
            }
        }
    }

    List<CanVirtualFeed> connections_;
    Mutex mutex_;
};

void CanVirtualFeed::State::write(const CanFrame &frame)
{
    bus_->broadcast(frame, alias<CanVirtualFeed>(this));
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
