/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/EchoNode>
#include <cc/Map>

namespace cc {
namespace can {

Ref<EchoNode> EchoNode::create(CanMedia *media, int nodeId, int timeout)
{
    auto node = new EchoNode{media, nodeId, timeout};
    node->init();
    return node;
}

EchoNode::EchoNode(CanMedia *media, int nodeId, int timeout):
    CanNode{media, nodeId, timeout},
    settings_{Settings::create()}
{}

String EchoNode::readServiceData(Selector::Key key) const
{
    return settings_->value(key);
}

void EchoNode::writeServiceData(Selector::Key key, const String &data)
{
    settings_->establish(key, data);
}

}} // namespace cc::can
