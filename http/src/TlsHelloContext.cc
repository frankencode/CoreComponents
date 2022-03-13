/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/TlsHelloContext>

namespace cc {

struct TlsHelloContext::State: public Object::State
{
    void init(const SocketAddress &peerAddress, TlsServiceSelector &&serviceSelector)
    {
        peerAddress_ = peerAddress;
        serviceSelector_ = std::move(serviceSelector);
    }

    TlsServerOptions selectService(const String &serverName)
    {
        return serviceSelector_(serverName, peerAddress_);
    }

    SocketAddress peerAddress_;
    TlsServiceSelector serviceSelector_;
};

TlsHelloContext::TlsHelloContext()
{
    initOncePerThread<State>();
}

SocketAddress TlsHelloContext::peerAddress() const
{
    return me().peerAddress_;
}

void TlsHelloContext::init(const SocketAddress &peerAddress, TlsServiceSelector &&serviceSelector)
{
    me().init(peerAddress, std::move(serviceSelector));
}

TlsServerOptions TlsHelloContext::selectService(const String &serverName)
{
    return me().selectService(serverName);
}

TlsHelloContext::State &TlsHelloContext::me()
{
    return Object::me.as<State>();
}

const TlsHelloContext::State &TlsHelloContext::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
