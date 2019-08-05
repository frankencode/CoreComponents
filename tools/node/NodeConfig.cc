/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/File>
#include <cc/Dir>
#include <cc/ResourceGuard>
#include <cc/Arguments>
#include "NodeConfigProtocol.h"
#include "ServiceRegistry.h"
#include "ErrorLog.h"
#include "NodeConfig.h"

namespace ccnode {

Ref<NodeConfig> NodeConfig::load(int argc, char **argv)
{
    Ref<Arguments> arguments = Arguments::parse(argc, argv);
    const StringList *items = arguments->items();

    MetaObject *nodePrototype = NodeConfigProtocol::instance()->nodePrototype_;
    arguments->validate(nodePrototype);

    Ref<MetaObject> config;
    String dirPath;

    if (items->count() > 0) {
        if (items->count() > 1)
            throw UsageError{"Handling multiple input arguments at once is not supported"};

        String path = items->at(0);
        if (Dir::exists(path)) {
            dirPath = path;
        }
        else {
            ResourceGuard context{path};
            config = yason::parse(File::open(path)->map(), NodeConfigProtocol::instance());
        }
    }

    if (!config) config = nodePrototype->clone();
    arguments->override(config);

    return new NodeConfig{config, dirPath};
}

Ref<NodeConfig> NodeConfig::load(const String &path)
{
    ResourceGuard context{path};

    return load(
        yason::parse(
            File::open(path)->map(),
            NodeConfigProtocol::instance()
        )
    );
}

Ref<NodeConfig> NodeConfig::load(MetaObject *config)
{
    return new NodeConfig{config};
}

NodeConfig::NodeConfig(MetaObject *config, const String &dirPath):
    directoryPath_{dirPath}
{
    String address = config->value("address");

    auto ports = List<int>::create();
    for (int p: Variant::cast<List<int> *>(config->value("port"))) ports->append(p);

    String protocol = config->value("protocol-family");

    ProtocolFamily family = ProtocolFamily::Unspecified;
    if (protocol->toLower() == "ipv6") family = ProtocolFamily::Internet6;
    else if (protocol->toLower() == "ipv4") family = ProtocolFamily::Internet4;

    address_ = SocketAddressList::create();
    if (address != "" && address != "*") {
        Ref<SocketAddressList> l = SocketAddress::resolve(address, "http", family, SocketType::Stream);
        for (SocketAddress *a: l) {
            for (int p: ports) {
                a->setPort(p);
                address_->append(a);
            }
        }
    }
    else {
        for (int p: ports) {
            if (family == ProtocolFamily::Unspecified) {
                address_->append(SocketAddress::create(ProtocolFamily::Internet4, "*", p));
                address_->append(SocketAddress::create(ProtocolFamily::Internet6, "*", p));
            }
            else
                address_->append(SocketAddress::create(family, "*", p));
        }
    }

    securePort_ = 443;
    for (const SocketAddress *a: address_) {
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

    if (directoryPath_ != "") {
        ServiceDefinition *service = serviceRegistry()->serviceByName("Directory");
        MetaObject *serviceConfig = service->configPrototype();
        serviceConfig->establish("host", "*");
        serviceConfig->establish("path", directoryPath_);
        serviceInstances_->append(service->createInstance(serviceConfig));
    }

    if (serviceInstances_->count() == 0) {
        ServiceDefinition *service = serviceRegistry()->serviceByName("Echo");
        MetaObject *serviceConfig = service->configPrototype();
        serviceConfig->establish("host", "*");
        serviceInstances_->append(service->createInstance(serviceConfig));
    }
}

ServiceInstance *NodeConfig::selectService(const String &host, const String &uri) const
{
    ServiceInstance *serviceInstance = nullptr;

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

} // namespace ccnode
