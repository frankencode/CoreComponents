/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXNODE_DISPATCHINSTANCE_H
#define FLUXNODE_DISPATCHINSTANCE_H

#include "ServiceInstance.h"
#include "WorkerPool.h"

namespace fluxnode
{

class NodeMaster;

class DispatchInstance: public ServiceInstance
{
public:
	static Ref<DispatchInstance> create(YasonObject *config);
	virtual Ref<ServiceDelegate> createDelegate(ServiceWorker *worker) const;

	inline WorkerPools *workerPools() const { return workerPools_; }

private:
	friend class NodeMaster;

	DispatchInstance(YasonObject *config);

	Ref<WorkerPools> workerPools_;
};

} // namespace fluxnode

#endif // FLUXNODE_DISPATCHINSTANCE_H
