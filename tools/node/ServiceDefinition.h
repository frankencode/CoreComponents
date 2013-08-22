/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FNODE_SERVICEDEFINITION_H
#define FNODE_SERVICEDEFINITION_H

#include <fkit/Yason.h>
#include <fkit/Pattern.h>
#include "ServiceDelegate.h"
#include "ServiceInstance.h"

namespace fnode
{

using namespace fkit;

class ServicePrototype: public YasonObject
{
public:
	static Ref<ServicePrototype> create(String className, YasonProtocol *protocol = 0);

protected:
	ServicePrototype(String className, YasonProtocol *protocol);
};

class ServiceWorker;

class ServiceDefinition: public Object
{
public:
	virtual ServicePrototype *configPrototype() const = 0;
	virtual Ref<ServiceInstance> createInstance(YasonObject *config) const = 0;
};

} // namespace fnode

#endif // FNODE_SERVICEDEFINITION_H
