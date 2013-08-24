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

namespace fnode
{

Ref<ServicePrototype> ServicePrototype::create(String className, YasonProtocol *protocol)
{
	return new ServicePrototype(className, protocol);
}

ServicePrototype::ServicePrototype(String className, YasonProtocol *protocol)
	: YasonObject(className, protocol)
{
	insert("concurrency", 8);
	insert("connection_timeout", 30.);
	insert("host", "");
	insert("uri", "");
	insert("error_log", LogPrototype::create());
	insert("access_log", LogPrototype::create());
}

} // namespace fnode
