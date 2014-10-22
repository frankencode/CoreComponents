/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/Singleton>
#include <flux/Process>
#include "LogPrototype.h"
#include "NodeConfigProtocol.h"

namespace fluxnode {

class NodePrototype: public MetaObject
{
public:
    static Ref<NodePrototype> create(MetaProtocol *protocol = 0, String className = "Node")
    {
        return new NodePrototype(className, protocol);
    }

protected:
    NodePrototype(String className, MetaProtocol *protocol):
        MetaObject(className, protocol)
    {
        insert("address", "localhost");
        insert("port", Process::isSuperUser() ? 80 : 8080);
        insert("protocol", "");
        insert("user", "");
        insert("version", "fluxnode/0.2.0");
        insert("daemon", false);
        insert("service_window", 30);
        insert("error_log", LogPrototype::create());
        insert("access_log", LogPrototype::create());
    }
};

NodeConfigProtocol::NodeConfigProtocol():
    nodeProtocol_(MetaProtocol::create())
{
    Ref<NodePrototype> nodePrototype = NodePrototype::create(nodeProtocol_);
    define(nodePrototype);
}

void NodeConfigProtocol::registerService(MetaObject *configPrototype)
{
    nodeProtocol_->define(configPrototype);
}

NodeConfigProtocol *configProtocol() { return Singleton<NodeConfigProtocol>::instance(); }

} // namespace fluxnode
