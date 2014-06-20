/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "LogPrototype.h"
#include "ServiceDefinition.h"

namespace fluxnode
{

Ref<ServicePrototype> ServicePrototype::create(String className, MetaProtocol *protocol)
{
	return new ServicePrototype(className, protocol);
}

ServicePrototype::ServicePrototype(String className, MetaProtocol *protocol)
	: MetaObject(className, protocol)
{
	insert("concurrency", 64);
	insert("connection_timeout", 5.);
	insert("host", "");
	insert("uri", "");
	insert("error_log",  LogPrototype::create());
	insert("access_log", LogPrototype::create());
	insert("visit_log",  LogPrototype::create());
}

} // namespace fluxnode
