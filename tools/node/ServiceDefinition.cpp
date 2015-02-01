/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "LogPrototype.h"
#include "ServiceDefinition.h"

namespace fluxnode {

Ref<ServicePrototype> ServicePrototype::create(String className, MetaProtocol *protocol)
{
    return new ServicePrototype(className, protocol);
}

ServicePrototype::ServicePrototype(String className, MetaProtocol *protocol):
    MetaObject(className, protocol)
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
