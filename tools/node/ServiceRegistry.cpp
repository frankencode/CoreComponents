/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/Singleton.h>
#include "NodeConfigProtocol.h"
#include "ServiceRegistry.h"

namespace fluxnode
{

ServiceRegistry::ServiceRegistry()
	: serviceByName_(ServiceByName::create())
{}

void ServiceRegistry::registerService(ServiceDefinition *service)
{
	serviceByName_->establish(service->configPrototype()->className(), service);
	configProtocol()->registerService(service->configPrototype());
}

ServiceDefinition *ServiceRegistry::serviceByName(String name) const
{
	return serviceByName_->value(name);
}

ServiceRegistry *serviceRegistry() { return Singleton<ServiceRegistry>::instance(); }

} // namespace fluxnode
