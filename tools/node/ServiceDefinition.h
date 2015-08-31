/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNODE_SERVICEDEFINITION_H
#define FLUXNODE_SERVICEDEFINITION_H

#include <flux/RegExp>
#include <flux/meta/yason>
#include "ServiceDelegate.h"
#include "ServiceInstance.h"

namespace fluxnode {

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
