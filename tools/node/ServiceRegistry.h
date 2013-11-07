/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXNODE_SERVICEREGISTRY_H
#define FLUXNODE_SERVICEREGISTRY_H

#include <flux/Map.h>
#include "ServiceDefinition.h"

namespace fnode
{

class ServiceRegistry: public Object, public Singleton<ServiceRegistry>
{
public:
	void registerService(ServiceDefinition *service);
	ServiceDefinition *serviceByName(String name) const;

private:
	friend class Singleton<ServiceRegistry>;

	ServiceRegistry();

	typedef Map< String, Ref<ServiceDefinition> > ServiceByName;
	Ref<ServiceByName> serviceByName_;
};

inline ServiceRegistry *serviceRegistry() { return ServiceRegistry::instance(); }

} // namespace fnode

#endif // FLUXNODE_SERVICEREGISTRY_H
