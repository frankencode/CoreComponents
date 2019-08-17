/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/DeliveryRegistry>

#include <cc/node/NodeConfigProtocol>
#include <cc/Singleton>

namespace cc {
namespace node {

DeliveryRegistry *DeliveryRegistry::instance()
{
    return Singleton<DeliveryRegistry>::instance();
}

DeliveryRegistry::DeliveryRegistry():
    serviceByName_{ServiceByName::create()}
{}

void DeliveryRegistry::registerService(DeliveryService *service)
{
    serviceByName_->establish(service->configPrototype()->className(), service);
    NodeConfigProtocol::instance()->registerService(service->configPrototype());
}

DeliveryService *DeliveryRegistry::serviceByName(const String &name) const
{
    return serviceByName_->value(name);
}

}} // namespace cc::node
