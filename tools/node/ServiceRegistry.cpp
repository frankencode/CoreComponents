/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "ConfigProtocol.h"
#include "ServiceRegistry.h"

namespace fnode
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

} // namespace fnode
