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
#include <cc/node/DirectoryService>
#include <cc/node/EchoService>
#include <cc/node/DeliveryInstance>
#include <cc/node/SystemLoggingService>
#include <cc/node/ForegroundLoggingService>
#include <cc/node/LoggingRegistry>
#include <cc/node/debug>
#include <cc/meta/yason>
#include <cc/File>
#include <cc/ResourceGuard>

namespace cc {
namespace node {

using namespace cc::net;
using namespace cc::meta;

Ref<MetaObject> NodeConfig::parse(const String &text)
{
    return yason::parse(text, NodeConfigProtocol::instance());
}

Ref<NodeConfig> NodeConfig::load(const MetaObject *config)
{
    return new NodeConfig{config};
}

const MetaObject *NodeConfig::prototype()
{
    return NodeConfigProtocol::instance()->nodePrototype();
}

Ref<NodeConfig> NodeConfig::load(const String &path)
{
    ResourceGuard context{path};
    return load(parse(File::open(path)->map()));
}

Ref<NodeConfig> NodeConfig::create()
{
    return load(prototype());
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

    securityConfig_ = HttpServerSecurity::load(Variant::cast<const MetaObject *>(config->value("security")));

    auto loadLoggingInstance = [=](Variant value) {
        const MetaObject *config = Variant::cast<const MetaObject *>(value);
        if (!config) {
            if (daemon_) config = LoggingRegistry::instance()->serviceByName(SystemLoggingService::name())->configPrototype();
            else config = LoggingRegistry::instance()->serviceByName(ForegroundLoggingService::name())->configPrototype();
        }
        return LoggingInstance::load(config);
    };

    errorLoggingInstance_ = loadLoggingInstance(config->value("error-log"));
    accessLoggingInstance_ = loadLoggingInstance(config->value("access-log"));

    deliveryInstances_ = DeliveryInstances::create();
    for (const MetaObject *child: config->children()) {
        const DeliveryService *service = DeliveryRegistry::instance()->serviceByName(child->className());
        deliveryInstances_->append(createDeliveryInstance(service, child));
    }
}

Ref<DeliveryInstance> NodeConfig::createDeliveryInstance(const DeliveryService *service, const MetaObject *config) const
{
    Ref<DeliveryInstance> instance = service->createInstance(config);
    if (!instance->errorLoggingInstance_) instance->errorLoggingInstance_ = errorLoggingInstance_;
    if (!instance->accessLoggingInstance_) instance->accessLoggingInstance_ = accessLoggingInstance_;
    return instance;
}

void NodeConfig::addDirectoryInstance(const String &path)
{
    const DeliveryService *service = DeliveryRegistry::instance()->serviceByName(DirectoryService::name());
    Ref<MetaObject> config = service->configPrototype()->clone();
    config->establish("host", "*");
    config->establish("path", path);
    deliveryInstances_->append(createDeliveryInstance(service, config));
}

void NodeConfig::addEchoInstance()
{
    const DeliveryService *service = DeliveryRegistry::instance()->serviceByName(EchoService::name());
    Ref<MetaObject> config = service->configPrototype()->clone();
    config->establish("host", "*");
    deliveryInstances_->append(createDeliveryInstance(service, config));
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
