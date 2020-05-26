/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/http/NodeConfig>
#include <cc/http/NodeConfigProtocol>
#include <cc/http/DeliveryRegistry>
#include <cc/http/DeliveryInstance>
#include <cc/http/SystemLoggingService>
#include <cc/http/ForegroundLoggingService>
#include <cc/http/LoggingRegistry>
#include <cc/http/debug>
#include <cc/meta/yason>
#include <cc/File>
#include <cc/ResourceGuard>

namespace cc {
namespace http {

using namespace cc::net;
using namespace cc::meta;

Ref<MetaObject> NodeConfig::parse(const string &text)
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

Ref<NodeConfig> NodeConfig::load(const string &path)
{
    ResourceGuard context{path};
    return load(parse(File::open(path)->map()));
}

Ref<NodeConfig> NodeConfig::create()
{
    return load(prototype());
}

NodeConfig::NodeConfig(const MetaObject *config):
    securePort_{0}
{
    string address = config->value("address");

    auto ports = Variant::cast<List<int> *>(config->value("port"));

    ProtocolFamily family = ProtocolFamily::Unspecified;
    {
        string s = string{config->value("family")}->toLower();
        if (s->toLower() == "ipv6") family = ProtocolFamily::Internet6;
        else if (s->toLower() == "ipv4") family = ProtocolFamily::Internet4;
        else if (s->toLower() == "local" || address->contains('/')) family = ProtocolFamily::Local;
    }

    forceSecureTransport_ = config->value("tls");

    address_ = SocketAddressList::create();
    if (family == ProtocolFamily::Local) {
        address_->append(SocketAddress{ProtocolFamily::Local, address});
    }
    else {
        if (address != "" && address != "*") {
            Ref<SocketAddressList> l = SocketAddress::queryConnectionInfo(address, "http", family, SocketType::Stream);
            for (SocketAddress &a: l) {
                for (int p: ports) {
                    a->setPort(p);
                    address_->append(a);
                }
            }
        }
        else {
            for (int p: ports) {
                if (family == ProtocolFamily::Unspecified) {
                    address_->append(SocketAddress{ProtocolFamily::Internet4, "*", p});
                    address_->append(SocketAddress{ProtocolFamily::Internet6, "*", p});
                }
                else
                    address_->append(SocketAddress{family, "*", p});
            }
        }

        securePort_ = 443;
        for (const SocketAddress &a: address_) {
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
    daemonName_ = config->value("daemon-name");
    pidPath_ = config->value("pid-file");
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

void NodeConfig::addDirectoryInstance(const string &path)
{
    const DeliveryService *service = DeliveryRegistry::instance()->serviceByName("Directory");
    Ref<MetaObject> config = service->configPrototype()->clone();
    config->establish("host", "*");
    config->establish("path", path);
    deliveryInstances_->append(createDeliveryInstance(service, config));
}

void NodeConfig::addEchoInstance()
{
    const DeliveryService *service = DeliveryRegistry::instance()->serviceByName("Echo");
    Ref<MetaObject> config = service->configPrototype()->clone();
    config->establish("host", "*");
    deliveryInstances_->append(createDeliveryInstance(service, config));
}

const DeliveryInstance *NodeConfig::selectService(const string &host, const string &uri) const
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

}} // namespace cc::http
