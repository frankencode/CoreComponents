/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FNODE_SERVICEINSTANCE_H
#define FNODE_SERVICEINSTANCE_H

#include <fkit/Pattern.h>
#include <fkit/List.h>
#include "LogConfig.h"

namespace fkit { class YasonObject; }

namespace fnode
{

using namespace fkit;

class ServiceDelegate;
class ServiceWorker;

class ServiceInstance: public Object
{
public:
	virtual Ref<ServiceDelegate> createDelegate(ServiceWorker *worker) const = 0;

	inline String serviceName() const { return serviceName_; }

	inline int concurrency() const { return concurrency_; }
	inline int stackSize() const { return stackSize_; }
	inline double connectionTimeout() const { return connectionTimeout_; }
	inline Pattern host() const { return host_; }
	inline Pattern uri() const { return uri_; }

	inline LogConfig *errorLogConfig() const { return errorLogConfig_; }
	inline LogConfig *accessLogConfig() const { return accessLogConfig_; }

protected:
	ServiceInstance(YasonObject *config);

	String serviceName_;

	int concurrency_;
	int stackSize_;
	double connectionTimeout_;
	Pattern host_;
	Pattern uri_;

	Ref<LogConfig> errorLogConfig_;
	Ref<LogConfig> accessLogConfig_;
};

typedef List< Ref<ServiceInstance> > ServiceInstances;

} // namespace fnode

#endif // FNODE_SERVICEINSTANCE_H
