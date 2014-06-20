/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "EchoDelegate.h"

namespace fluxnode
{

class EchoInstance: public ServiceInstance
{
public:
	static Ref<EchoInstance> create(MetaObject *config) {
		return new EchoInstance(config);
	}

	virtual Ref<ServiceDelegate> createDelegate(ServiceWorker *worker) const {
		return EchoDelegate::create(worker);
	}

private:
	EchoInstance(MetaObject *config)
		: ServiceInstance(config)
	{}
};

} // namespace fluxnode
