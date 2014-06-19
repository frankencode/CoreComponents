/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXNODE_SERVICEINSTANCE_H
#define FLUXNODE_SERVICEINSTANCE_H

#include <flux/Pattern.h>
#include <flux/List.h>
#include "LogConfig.h"

namespace flux {
namespace yason {
class YasonObject;
}}

namespace fluxnode
{

using namespace flux;
using namespace flux::yason;

class ServiceDelegate;
class ServiceWorker;

class ServiceInstance: public Object
{
public:
	virtual Ref<ServiceDelegate> createDelegate(ServiceWorker *worker) const = 0;

	inline String serviceName() const { return serviceName_; }

	inline int concurrency() const { return concurrency_; }
	inline double connectionTimeout() const { return connectionTimeout_; }
	inline Pattern host() const { return host_; }
	inline Pattern uri() const { return uri_; }

	inline LogConfig *errorLogConfig() const { return errorLogConfig_; }
	inline LogConfig *accessLogConfig() const { return accessLogConfig_; }
	inline LogConfig *visitLogConfig() const { return visitLogConfig_; }

protected:
	ServiceInstance(YasonObject *config);

	String serviceName_;

	int concurrency_;
	double connectionTimeout_;
	Pattern host_;
	Pattern uri_;

	Ref<LogConfig> errorLogConfig_;
	Ref<LogConfig> accessLogConfig_;
	Ref<LogConfig> visitLogConfig_;
};

typedef List< Ref<ServiceInstance> > ServiceInstances;

} // namespace fluxnode

#endif // FLUXNODE_SERVICEINSTANCE_H
