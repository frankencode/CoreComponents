/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Singleton>
#include "NodeConfigProtocol.h"
#include "DeliveryRegistry.h"

namespace ccnode {

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

} // namespace ccnode
