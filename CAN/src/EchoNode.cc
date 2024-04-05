/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/can_open/EchoNode>
#include <cc/can_open/Crc16Sink>
#include <cc/Map>
#include <cc/debugging>

namespace cc::can_open {

struct EchoNode::State final: public Node::State
{
    State(const CanMedia &media, int nodeId, int timeout):
        Node::State{media, nodeId, timeout}
    {}

    Bytes readObject(Selector selector) const override
    {
        Bytes data = settings_.value(selector);
        return data;
    }

    void writeObject(Selector selector, const Bytes &data) override
    {
        CC_INSPECT(data.count());
        CC_INSPECT(crc16(data));
        settings_.establish(selector, data);
    }

    Map<Selector, Bytes> settings_;
};

EchoNode::EchoNode(const CanMedia &media, int nodeId, int timeout):
    Node{new State{media, nodeId, timeout}}
{}

} // namespace cc::can_open
