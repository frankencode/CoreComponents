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

    auto ports = List<int>::create();
    for (int p: Variant::cast<List<int> *>(config->value("port"))) ports->append(p);

    String protocol = config->value("protocol-family");

    int family = AF_UNSPEC;
    if (protocol->toLower() == "ipv6") family = AF_INET6;
    else if (protocol->toLower() == "ipv4") family = AF_INET;

    address_ = SocketAddressList::create();
    if (address != "" && address != "*") {
        Ref<SocketAddressList> l = SocketAddress::resolve(address, "http", family, SOCK_STREAM);
        for (SocketAddress *a: l) {
            for (int p: ports) {
                a->setPort(p);
                address_->append(a);
            }
        }
    }
    else {
        for (int p: ports) {
            if (family == AF_UNSPEC) {
                address_->append(SocketAddress::create(AF_INET, "*", p));
                address_->append(SocketAddress::create(AF_INET6, "*", p));
            }
            else
                address_->append(SocketAddress::create(family, "*", p));
        }
    }

    securePort_ = 443;
    for (SocketAddress *a: address_) {
        if (a->port() % 80 != 0) {
            securePort_ = a->port();
            break;
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

    securityConfig_ = SecurityConfig::load(Variant::cast<MetaObject *>(config->value("security")));
    errorLogConfig_ = LogConfig::load(Variant::cast<MetaObject *>(config->value("error-log")));
    accessLogConfig_ = LogConfig::load(Variant::cast<MetaObject *>(config->value("access-log")));

    serviceInstances_ = ServiceInstances::create();
    if (config->hasChildren()) {
        for (MetaObject *child: config->children()) {
            ServiceDefinition *service = serviceRegistry()->serviceByName(child->className());
            serviceInstances_->append(service->createInstance(child));
        }
    }
}

ServiceInstance *NodeConfig::selectService(String host, String uri) const
{
    ServiceInstance *serviceInstance = 0;

    for (ServiceInstance *candidate: serviceInstances_)
    {
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
