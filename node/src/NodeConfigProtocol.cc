/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/NodeConfigProtocol>
#include <cc/node/SecurityPrototype>
#include <cc/node/LogPrototype>
#include <cc/Singleton>
#include <cc/Process>

namespace cc {
namespace node {

class NodePrototype: public MetaPrototype
{
public:
    static Ref<NodePrototype> create(MetaProtocol *protocol = nullptr, const String &className = "Node")
    {
        return new NodePrototype{className, protocol};
    }

protected:
    NodePrototype(const String &className, MetaProtocol *protocol):
        MetaPrototype{className, protocol}
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
        establish("version", "ccnode");
        establish("daemon", false);
        establish("daemon-name", "ccnode");
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

NodeConfigProtocol *NodeConfigProtocol::instance()
{
    return Singleton<NodeConfigProtocol>::instance();
}

NodeConfigProtocol::NodeConfigProtocol():
    nodeProtocol_{MetaProtocol::create()},
    nodePrototype_{NodePrototype::create(nodeProtocol_)}
{
    define(nodePrototype_);
}

void NodeConfigProtocol::registerService(const MetaPrototype *configPrototype)
{
    nodeProtocol_->define(configPrototype);
}

}} // namespace cc::node
