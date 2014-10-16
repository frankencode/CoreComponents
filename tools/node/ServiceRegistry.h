/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNODE_SERVICEREGISTRY_H
#define FLUXNODE_SERVICEREGISTRY_H

#include <flux/Map>
#include "ServiceDefinition.h"

namespace flux { template<class> class Singleton; }

namespace fluxnode
{

class ServiceRegistry: public Object
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

ServiceRegistry *serviceRegistry();

} // namespace fluxnode

#endif // FLUXNODE_SERVICEREGISTRY_H
