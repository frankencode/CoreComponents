/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Singleton>
#include <cc/Process>
#include <cc/Bundle>
#include "SecurityPrototype.h"
#include "LogPrototype.h"
#include "NodeConfigProtocol.h"

namespace ccnode {

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
        bool superUser = Process::isSuperUser();
        establish("address", "*");
        establish("port",
            superUser ?
            (List<int>::create() << 80 << 443) :
            (List<int>::create() << 8080 << 4443)
        );
        establish("protocol-family", "");
        establish("user", "");
        establish("group", "");
        establish("version", "ccnode" /*"/" CC_BUNDLE_VERSION*/);
        establish("daemon", false);
        establish("concurrency",
            #ifdef NDEBUG
            256
            #else
            16
            #endif
        );
        establish("service-window", 30);
        establish("connection-limit", 32);
        establish("connection-timeout", 10.);
        establish("security", SecurityPrototype::create());
        establish("error-log", LogPrototype::create());
        establish("access-log", LogPrototype::create());
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

} // namespace ccnode
