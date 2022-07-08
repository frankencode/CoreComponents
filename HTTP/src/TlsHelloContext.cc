/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TlsHelloContext>

namespace cc {

struct TlsHelloContext::State: public Object::State
{
    void init(const SocketAddress &peerAddress, TlsServiceSelector &&serviceSelector)
    {
        peerAddress_ = peerAddress;
        serviceSelector_ = move(serviceSelector);
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
    me().init(peerAddress, move(serviceSelector));
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
