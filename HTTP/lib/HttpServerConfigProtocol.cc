/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/HttpServerConfigProtocol>
#include <cc/HttpLoggingServiceRegistry>
#include <cc/TlsServerOptionsPrototype>
#include <cc/MetaProtocolState>
#include <cc/Process>

namespace cc {

class NodePrototype: public MetaPrototype
{
public:
    explicit NodePrototype(const MetaProtocol &protocol):
        MetaPrototype{"Node", protocol}
    {
        bool superUser = Process::isSuperUser();
        establish("address", "*");
        establish("port", superUser ? List<long>{80, 443} : List<long>{8080, 4443});
        establish("family", "");
        establish("tls", false);
        establish("user", "");
        establish("group", "");
        establish("version", "cc_node");
        establish("daemon", false);
        establish("daemon-name", "cc_node");
        establish("pid-file", "");
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
        establish("security", TlsServerOptionsPrototype{}.as<MetaPrototype>());
        establish("error-log", HttpLoggingServiceRegistry{}.loggingProtocol());
        establish("access-log", HttpLoggingServiceRegistry{}.loggingProtocol());
    }
};

struct HttpServerConfigProtocol::State: public MetaProtocol::State
{
    State():
        nodeProtocol_{New{}},
        configPrototype_{nodeProtocol_}
    {
        define(configPrototype_);
    }

    MetaProtocol nodeProtocol_;
    NodePrototype configPrototype_;
};

HttpServerConfigProtocol::HttpServerConfigProtocol()
{
    initOnce<State>();
}

MetaObject HttpServerConfigProtocol::configPrototype() const
{
    return me().configPrototype_;
}

void HttpServerConfigProtocol::registerService(const MetaPrototype &configPrototype)
{
    me().nodeProtocol_.define(configPrototype);
}

HttpServerConfigProtocol::State &HttpServerConfigProtocol::me()
{
    return Object::me.as<State>();
}

const HttpServerConfigProtocol::State &HttpServerConfigProtocol::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
