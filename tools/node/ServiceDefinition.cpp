/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "ServiceDefinition.h"

namespace fnode
{

Ref<ServicePrototype> ServicePrototype::create(String className)
{
	return new ServicePrototype(className);
}

ServicePrototype::ServicePrototype(String className)
	: YasonObject(className)
{
	insert("concurrency", 8);
	insert("stack_size", 65536);
	insert("max_connections", 256);
	insert("max_connections_per_client", 8);
	insert("connection_timeout", 30.);
	insert("host", "");
	insert("uri", "");
}

} // namespace fnode
