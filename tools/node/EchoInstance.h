/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
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
