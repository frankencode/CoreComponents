/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/MetaObject.h>
#include "LogConfig.h"
#include "ServiceInstance.h"

namespace fluxnode
{

ServiceInstance::ServiceInstance(MetaObject *config)
	: serviceName_(config->className()),
	  concurrency_(config->value("concurrency")),
	  connectionTimeout_(config->value("connection_timeout")),
	  host_(String(config->value("host"))),
	  uri_(String(config->value("uri"))),
	  errorLogConfig_(LogConfig::load(cast<MetaObject>(config->value("error_log")))),
	  accessLogConfig_(LogConfig::load(cast<MetaObject>(config->value("access_log")))),
	  visitLogConfig_(LogConfig::load(cast<MetaObject>(config->value("visit_log"))))
{}

} // namespace fluxnode
