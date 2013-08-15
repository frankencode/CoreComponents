/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "DispatchDelegate.h"
#include "DispatchInstance.h"

namespace fnode
{

Ref<DispatchInstance> DispatchInstance::create(YasonObject *config) {
	return new DispatchInstance(config);
}

Ref<ServiceDelegate> DispatchInstance::createDelegate(ServiceWorker *worker) const {
	return DispatchDelegate::create(worker);
}

DispatchInstance::DispatchInstance(YasonObject *config)
	: ServiceInstance(config)
{}

} // namespace fnode
