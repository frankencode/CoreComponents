/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/File>
#include <cc/Dir>
#include <cc/ResourceGuard>
#include <cc/Singleton>
#include <cc/Arguments>
#include "NodeConfigProtocol.h"
#include "ServiceRegistry.h"
#include "ErrorLog.h"
#include "NodeConfig.h"

namespace ccnode {

NodeConfig::NodeConfig()
{}

void NodeConfig::load(int argc, char **argv)
{
    Ref<Arguments> arguments = Arguments::parse(argc, argv);
    Ref<StringList> items = arguments->items();

    MetaObject *nodePrototype = configProtocol()->lookup("Node");
    arguments->validate(nodePrototype);

    Ref<MetaObject> config;

    if (items->count() > 0) {
        if (items->count() > 1)
            throw UsageError("Handling multiple input arguments at once is not supported");

        String path = items->at(0);
        if (Dir::exists(path)) {
            directoryPath_ = path;
        }
        else {
            ResourceGuard context(path);
            config = yason::parse(File::open(path)->map(), configProtocol());
        }
    }

    if (!config) config = nodePrototype->clone();
    arguments->override(config);

    String address = config->value("address");

    typedef List<int> PortList;
    Ref<PortList> ports = PortList::create();
    {
        PortList *l = cast<PortList>(config->value("port"));
        for (int i = 0; i < l->count(); ++i)
            ports->append(int(l->at(i)));
    }

    String protocol = config->value("protocol-family");

    int family = AF_UNSPEC;
    if (protocol->downcase() == "ipv6") family = AF_INET6;
    else if (protocol->downcase() == "ipv4") family = AF_INET;

    address_ = SocketAddressList::create();
    if (address != "" && address != "*") {
        Ref<SocketAddressList> l = SocketAddress::resolve(address, "http", family, SOCK_STREAM);

        for (int i = 0; i < l->count(); ++i) {
            for (int j = 0; j < ports->count(); ++j) {
                Ref<SocketAddress> a = SocketAddress::copy(l->at(i));
                a->setPort(ports->at(j));
                address_->append(a);
            }
        }
    }
    else {
        if (family == AF_UNSPEC) {
            for (int j = 0; j < ports->count(); ++j) {
                address_->append(SocketAddress::create(AF_INET, "*", ports->at(j)));
                address_->append(SocketAddress::create(AF_INET6, "*", ports->at(j)));
            }
        }
        else {
            for (int j = 0; j < ports->count(); ++j) {
                address_->append(SocketAddress::create(family, "*", ports->at(j)));
            }
        }
    }

    securePort_ = 443;
    {
        SocketAddressList *l = address_;
        for (int i = 0; i < l->count(); ++i) {
            SocketAddress *a = l->at(i);
            if (a->port() % 80 != 0) {
                securePort_ = a->port();
                break;
            }
        }
    }

    user_ = config->value("user");
    group_ = config->value("group");
    version_ = config->value("version");
    daemon_ = config->value("daemon");
    concurrency_ = config->value("concurrency");
    serviceWindow_ = config->value("service-window");
    connectionLimit_ = config->value("connection-limit");
    connectionTimeout_ = config->value("connection-timeout");

    securityConfig_ = SecurityConfig::load(cast<MetaObject>(config->value("security")));
    errorLogConfig_ = LogConfig::load(cast<MetaObject>(config->value("error-log")));
    accessLogConfig_ = LogConfig::load(cast<MetaObject>(config->value("access-log")));

    serviceInstances_ = ServiceInstances::create();
    if (config->hasChildren()) {
        for (int i = 0; i < config->children()->count(); ++i) {
            MetaObject *child = config->children()->at(i);
            ServiceDefinition *service = serviceRegistry()->serviceByName(child->className());
            serviceInstances_->append(service->createInstance(child));
        }
    }
}

ServiceInstance *NodeConfig::selectService(String host, String uri) const
{
    ServiceInstance *serviceInstance = 0;

    for (int i = 0; i < serviceInstances_->count(); ++i)
    {
        ServiceInstance *candidate = serviceInstances_->at(i);

        if (
            candidate->host()->match(host)->valid() ||
            (uri != "" && candidate->uri()->match(uri)->valid())
        ) {
            serviceInstance = candidate;
            break;
        }
    }

    CCNODE_DEBUG() << "Service for host = \"" << host << "\", uri = \"" << uri << "\": " << (serviceInstance ? serviceInstance->serviceName() : str("Nothing matches")) << nl;

    return serviceInstance;
}


NodeConfig *nodeConfig() { return Singleton<NodeConfig>::instance(); }

} // namespace ccnode
