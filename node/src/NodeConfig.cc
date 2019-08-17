/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/NodeConfig>
#include <cc/node/NodeConfigProtocol>
#include <cc/node/DeliveryRegistry>
#include <cc/node/ErrorLog>
#include <cc/File>
#include <cc/Dir>
#include <cc/ResourceGuard>
#include <cc/Arguments>

namespace cc {
namespace node {

using namespace cc::net;
using namespace cc::meta;

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

    Ref<NodeConfig> nodeConfig = new NodeConfig{config};

    if (dirPath != "") {
        DeliveryService *service = DeliveryRegistry::instance()->serviceByName("Directory");
        MetaObject *serviceConfig = service->configPrototype();
        serviceConfig->establish("host", "*");
        serviceConfig->establish("path", dirPath);
        nodeConfig->deliveryInstances()->append(service->createInstance(serviceConfig));
    }

    if (nodeConfig->deliveryInstances()->count() == 0) {
        DeliveryService *service = DeliveryRegistry::instance()->serviceByName("Echo");
        MetaObject *serviceConfig = service->configPrototype();
        serviceConfig->establish("host", "*");
        nodeConfig->deliveryInstances()->append(service->createInstance(serviceConfig));
    }

    return nodeConfig;
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

Ref<NodeConfig> NodeConfig::load(const MetaObject *config)
{
    return new NodeConfig{config};
}

Ref<NodeConfig> NodeConfig::create()
{
    return NodeConfig::load(
        NodeConfigProtocol::instance()->nodePrototype_
    );
}

NodeConfig::NodeConfig(const MetaObject *config)
{
    String address = config->value("address");

    auto ports = Variant::cast<List<int> *>(config->value("port"));
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
    daemonName_ = config->value("daemon-name");
    concurrency_ = config->value("concurrency");
    serviceWindow_ = config->value("service-window");
    connectionLimit_ = config->value("connection-limit");
    connectionTimeout_ = config->value("connection-timeout");

    securityConfig_ = HttpServerSecurity::load(Variant::cast<MetaObject *>(config->value("security")));
    errorLogConfig_ = LogConfig::load(Variant::cast<MetaObject *>(config->value("error-log")));
    accessLogConfig_ = LogConfig::load(Variant::cast<MetaObject *>(config->value("access-log")));

    deliveryInstances_ = DeliveryInstances::create();
    if (config->hasChildren()) {
        for (MetaObject *child: config->children()) {
            DeliveryService *service = DeliveryRegistry::instance()->serviceByName(child->className());
            deliveryInstances_->append(service->createInstance(child));
        }
    }
}

const DeliveryInstance *NodeConfig::selectService(const String &host, const String &uri) const
{
    const DeliveryInstance *deliveryInstance = nullptr;

    for (const DeliveryInstance *candidate: deliveryInstances_)
    {
        if (
            candidate->host()->match(host)->valid() ||
            (uri != "" && candidate->uri()->match(uri)->valid())
        ) {
            deliveryInstance = candidate;
            break;
        }
    }

    // FIXME: wrong place for issueing this error message
    CCNODE_DEBUG() << "Service for host = \"" << host << "\", uri = \"" << uri << "\": " << (deliveryInstance ? deliveryInstance->serviceName() : str("Nothing matches")) << nl;

    return deliveryInstance;
}

}} // namespace cc::node
