/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/HttpServerConfig>
#include <cc/HttpServerConfigProtocol>
#include <cc/HttpServer>
#include <cc/HttpLoggingServiceRegistry>
#include <cc/HttpServiceRegistry>
#include <cc/httpLogging>
#include <cc/TlsSessionCache>
#include <cc/yason>

namespace cc {

struct HttpServerConfig::State: public Object::State
{
    explicit State(const MetaObject &config)
    {
        HttpServer::loadPlugins();

        String address = config("address").to<String>();

        List<long> ports = config("port").to<List<long>>();

        ProtocolFamily family = ProtocolFamily::Unspec;
        {
            String s = config("family").to<String>().downcased();
            if (s == "ipv6") family = ProtocolFamily::Inet6;
            else if (s == "ipv4") family = ProtocolFamily::Inet4;
            else if (s == "local" || address.contains('/')) family = ProtocolFamily::Local;
        }

        forceSecureTransport_ = config("tls").to<bool>();

        if (family == ProtocolFamily::Local) {
            address_.append(SocketAddress{ProtocolFamily::Local, address});
        }
        else {
            if (address != "" && address != "*") {
                List<SocketAddress> l = SocketAddress::queryConnectionInfo(address, "http", family, SocketType::Stream);
                for (SocketAddress &a: l) {
                    for (int p: ports) {
                        a.setPort(p);
                        address_.append(a);
                    }
                }
            }
            else {
                for (int p: ports) {
                    if (family == ProtocolFamily::Unspec) {
                        address_.append(SocketAddress{ProtocolFamily::Inet4, "*", p});
                        address_.append(SocketAddress{ProtocolFamily::Inet6, "*", p});
                    }
                    else
                        address_.append(SocketAddress{family, "*", p});
                }
            }

            securePort_ = 443;
            for (const SocketAddress &a: address_) {
                if (a.port() % 80 != 0) {
                    securePort_ = a.port();
                    break;
                }
            }
        }

        user_ = config("user").to<String>();
        group_ = config("group").to<String>();
        version_ = config("version").to<String>();
        daemon_ = config("daemon").to<bool>();
        daemonName_ = config("daemon-name").to<String>();
        pidPath_ = config("pid-file").to<String>();
        concurrency_ = config("concurrency").to<long>();
        serviceWindow_ = config("service-window").to<long>();
        connectionLimit_ = config("connection-limit").to<long>();
        connectionTimeout_ = config("connection-timeout").to<double>();
        connectionTimeoutMs_ = 1000 * connectionTimeout_;

        tlsOptions_ = TlsServerOptions{config("security").to<MetaObject>()};

        auto loadLoggingInstance = [=,this](const Variant &value) {
            MetaObject config = value.to<MetaObject>();
            if (!config) {
                if (daemon_) config = HttpLoggingServiceRegistry{}.serviceByName(syslogLoggingServiceName()).configPrototype();
                else config = HttpLoggingServiceRegistry{}.serviceByName(foregroundLoggingServiceName()).configPrototype();
            }
            return HttpLoggingServiceInstance{config};
        };

        errorLoggingInstance_ = loadLoggingInstance(config("error-log"));
        accessLoggingInstance_ = loadLoggingInstance(config("access-log"));

        for (const MetaObject &child: config.children()) {
            HttpService service = HttpServiceRegistry{}.serviceByName(child.className());
            serviceInstances_.append(createDeliveryInstance(service, child));
        }
    }

    HttpServiceInstance createDeliveryInstance(const HttpService &service, const MetaObject &config)
    {
        HttpServiceInstance instance = service.createInstance(config);
        if (!instance->errorLoggingInstance_) instance->errorLoggingInstance_ = errorLoggingInstance_;
        if (!instance->accessLoggingInstance_) instance->accessLoggingInstance_ = accessLoggingInstance_;
        return instance;
    }

    void addDirectoryInstance(const String &path)
    {
        HttpService service = HttpServiceRegistry{}.serviceByName("Directory");
        MetaObject config = service.configPrototype().clone();
        config.establish("host", "*");
        config.establish("path", path);
        serviceInstances_.append(createDeliveryInstance(service, config));
    }

    void addEchoInstance()
    {
        HttpService service = HttpServiceRegistry{}.serviceByName("Echo");
        MetaObject config = service.configPrototype().clone();
        config.establish("host", "*");
        serviceInstances_.append(createDeliveryInstance(service, config));
    }

    List<SocketAddress> address_;
    bool forceSecureTransport_;
    int securePort_;
    String user_;
    String group_;
    String version_;
    bool daemon_;
    String daemonName_;
    String pidPath_;
    long concurrency_;
    long serviceWindow_;
    long connectionLimit_;
    double connectionTimeout_;
    int connectionTimeoutMs_;

    TlsServerOptions tlsOptions_;

    HttpLoggingServiceInstance errorLoggingInstance_;
    HttpLoggingServiceInstance accessLoggingInstance_;

    List<HttpServiceInstance> serviceInstances_;
};

MetaObject HttpServerConfig::prototype()
{
    return HttpServerConfigProtocol{}.configPrototype();
}

MetaObject HttpServerConfig::parse(const String &text)
{
    HttpServer::loadPlugins();
    return yasonParse(text, HttpServerConfigProtocol{}).to<MetaObject>();
}

HttpServerConfig::HttpServerConfig(const MetaObject &config):
    Object{new State{config}}
{}

HttpServerConfig::HttpServerConfig(const String &text):
    HttpServerConfig{parse(text)}
{}

void HttpServerConfig::addDirectoryInstance(const String &path)
{
    me().addDirectoryInstance(path);
}

void HttpServerConfig::addEchoInstance()
{
    me().addEchoInstance();
}

List<SocketAddress> HttpServerConfig::address() const
{
    return me().address_;
}

bool HttpServerConfig::forceSecureTransport() const
{
    return me().forceSecureTransport_;
}

long HttpServerConfig::securePort() const
{
    return me().securePort_;
}

String HttpServerConfig::user() const
{
    return me().user_;
}

String HttpServerConfig::group() const
{
    return me().group_;
}

String HttpServerConfig::version() const
{
    return me().version_;
}

bool HttpServerConfig::daemon() const
{
    return me().daemon_;
}

String HttpServerConfig::daemonName() const
{
    return me().daemonName_;
}

String HttpServerConfig::pidPath() const
{
    return me().pidPath_;
}

long HttpServerConfig::concurrency() const
{
    return me().concurrency_;
}

long HttpServerConfig::serviceWindow() const
{
    return me().serviceWindow_;
}

long HttpServerConfig::connectionLimit() const
{
    return me().connectionLimit_;
}

double HttpServerConfig::connectionTimeout() const
{
    return me().connectionTimeout_;
}

int HttpServerConfig::connectionTimeoutMs() const
{
    return me().connectionTimeoutMs_;
}

TlsServerOptions HttpServerConfig::tlsOptions() const
{
    return me().tlsOptions_;
}

const HttpLoggingServiceInstance &HttpServerConfig::errorLoggingInstance() const
{
    return me().errorLoggingInstance_;
}

const HttpLoggingServiceInstance &HttpServerConfig::accessLoggingInstance() const
{
    return me().accessLoggingInstance_;
}

List<HttpServiceInstance> HttpServerConfig::serviceInstances() const
{
    return me().serviceInstances_;
}

HttpServiceInstance HttpServerConfig::selectService(const String &host, const String &uri) const
{
    HttpServiceInstance serviceInstance;

    for (const HttpServiceInstance &candidate: me().serviceInstances_)
    {
        if (
            candidate.host().match(host) ||
            (uri != "" && candidate.uri().match(uri))
        ) {
            serviceInstance = candidate;
            break;
        }
    }

    return serviceInstance;
}

HttpServerConfig::State &HttpServerConfig::me()
{
    return Object::me.as<State>();
}

const HttpServerConfig::State &HttpServerConfig::me() const
{
    return Object::me.as<State>();
}

} // namespapace cc
