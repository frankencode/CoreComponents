/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "ServiceRegistry.h"
#include "ServiceDefinition.h"
#include "WorkerPool.h"
#include "DispatchInstance.h"

namespace fluxnode
{

class DispatchService: public ServiceDefinition
{
public:
	static Ref<DispatchService> create() {
		return new DispatchService;
	}

	virtual ServicePrototype *configPrototype() const { return configPrototype_; }
	virtual Ref<ServiceInstance> createInstance(YasonObject *config) const { return DispatchInstance::create(config); }

private:
	DispatchService()
		: configPrototype_(ServicePrototype::create("Dispatch"))
	{
		configPrototype_->remove("connection_timeout");
	}

	Ref<ServicePrototype> configPrototype_;
};

class DispatchAnnouncer {
public:
	DispatchAnnouncer()
	{
		static bool done = false;
		if (done) return;
		Ref<DispatchService> dispatchService = DispatchService::create();
		serviceRegistry()->registerService(dispatchService);
		done = true;
	}
};

namespace { DispatchAnnouncer announcer; }

} // namespace fluxnode
