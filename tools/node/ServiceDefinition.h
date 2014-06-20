/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXNODE_SERVICEDEFINITION_H
#define FLUXNODE_SERVICEDEFINITION_H

#include <flux/Pattern.h>
#include <flux/yason.h>
#include "ServiceDelegate.h"
#include "ServiceInstance.h"

namespace fluxnode
{

using namespace flux;

class ServicePrototype: public MetaObject
{
public:
	static Ref<ServicePrototype> create(String className, MetaProtocol *protocol = 0);

protected:
	ServicePrototype(String className, MetaProtocol *protocol);
};

class ServiceWorker;

class ServiceDefinition: public Object
{
public:
	virtual ServicePrototype *configPrototype() const = 0;
	virtual Ref<ServiceInstance> createInstance(MetaObject *config) const = 0;
};

} // namespace fluxnode

#endif // FLUXNODE_SERVICEDEFINITION_H
