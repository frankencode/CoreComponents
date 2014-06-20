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
#include "EchoInstance.h"

namespace fluxnode
{

class EchoService: public ServiceDefinition
{
public:
	static Ref<EchoService> create() {
		return new EchoService;
	}

	virtual ServicePrototype *configPrototype() const { return configPrototype_; }
	virtual Ref<ServiceInstance> createInstance(MetaObject *config) const { return EchoInstance::create(config); }

private:
	EchoService()
		: configPrototype_(ServicePrototype::create("Echo"))
	{}

	Ref<ServicePrototype> configPrototype_;
};

class EchoAnnouncer {
public:
	EchoAnnouncer()
	{
		static bool done = false;
		if (done) return;
		Ref<EchoService> echoService = EchoService::create();
		serviceRegistry()->registerService(echoService);
		done = true;
	}
};

namespace { EchoAnnouncer announcer; }

} // namespace fluxnode
