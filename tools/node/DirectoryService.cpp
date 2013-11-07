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
#include "DirectoryInstance.h"

namespace fluxnode
{

class DirectoryService: public ServiceDefinition
{
public:
	static Ref<DirectoryService> create() {
		return new DirectoryService;
	}

	virtual ServicePrototype *configPrototype() const { return configPrototype_; }
	virtual Ref<ServiceInstance> createInstance(YasonObject *config) const { return DirectoryInstance::create(config); }

private:
	DirectoryService()
		: configPrototype_(ServicePrototype::create("Directory"))
	{
		configPrototype_->insert("path", "");
	}

	Ref<ServicePrototype> configPrototype_;
};

class DirectoryAnnouncer {
public:
	DirectoryAnnouncer() {
		static bool done = false;
		if (done) return;
		Ref<DirectoryService> directoryService = DirectoryService::create();
		serviceRegistry()->registerService(directoryService);
		done = true;
	}
};

namespace { DirectoryAnnouncer announcer; }

} // namespace fluxnode
