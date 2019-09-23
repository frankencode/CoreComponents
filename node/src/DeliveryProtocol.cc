/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/DeliveryProtocol>
#include <cc/node/LoggingRegistry>
#include <cc/Singleton>

namespace cc {
namespace node {

const DeliveryProtocol *DeliveryProtocol::defaultInstance()
{
    return Singleton<DeliveryProtocol>::instance();
}

DeliveryProtocol::DeliveryProtocol():
    MetaProtocol{LoggingRegistry::instance()->loggingProtocol()}
{}

}} // namespace cc::node
