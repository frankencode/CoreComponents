/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Singleton>
#include "NodeConfigProtocol.h"
#include "ServiceRegistry.h"

namespace ccnode {

ServiceRegistry::ServiceRegistry():
    serviceByName_{ServiceByName::create()}
{}

void ServiceRegistry::registerService(ServiceDefinition *service)
{
    serviceByName_->establish(service->configPrototype()->className(), service);
    NodeConfigProtocol::instance()->registerService(service->configPrototype());
}

ServiceDefinition *ServiceRegistry::serviceByName(String name) const
{
    return serviceByName_->value(name);
}

ServiceRegistry *serviceRegistry() { return Singleton<ServiceRegistry>::instance(); }

} // namespace ccnode
