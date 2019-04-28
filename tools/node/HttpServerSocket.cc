/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <stddef.h>
#include <unistd.h>
#include <cc/assert>
#include <cc/ThreadLocalSingleton>
#include <cc/System>
#include <cc/NullStream>
#include <cc/http/exceptions>
#include "ErrorLog.h"
#include "NodeConfig.h"
#include "SecurityConfig.h"
#include "SecurityMaster.h"
#include "HttpServerSocket.h"

namespace ccnode {

Ref<HttpServerSocket> HttpServerSocket::accept(StreamSocket *listeningSocket)
{
    Ref<HttpServerSocket> client =
        new HttpServerSocket(
            SocketAddress::create(listeningSocket->address()->family()),
            (listeningSocket->address()->port() % 80 == 0) ? 0 : Secure
        );
    client->fd_ = StreamSocket::accept(listeningSocket, client->address_);
    client->connected_ = true;
    client->mode_ |= Connected | ((client->mode_ & Secure) ? 0 : Open);
    client->initSession();
    return client;
}

HttpServerSocket::HttpServerSocket(const SocketAddress *address, int mode):
    HttpSocket(address, mode)
{}

HttpServerSocket::~HttpServerSocket()
{
    if (mode_ & Secure)
    {
        if (mode_ & Open) {
            int ret = gnutls_bye(session_, GNUTLS_SHUT_WR);
            if (ret != GNUTLS_E_SUCCESS)
                CCNODE_ERROR() << gnutls_strerror(ret) << nl;
        }

        if (mode_ & Connected)
            gnutls_deinit(session_);
    }
}

void HttpServerSocket::initSession()
{
    CC_ASSERT(mode_ & Connected);

    t0_ = System::now();
    te_ = t0_ + NodeConfig::instance()->connectionTimeout();

    if (!(mode_ & Secure)) return;

    CC_ASSERT(!(mode_ & Open));

    gnuTlsCheckSuccess(gnutls_init(&session_, GNUTLS_SERVER));
    if (NodeConfig::instance()->security()->hasCredentials())
        gnuTlsCheckSuccess(gnutls_credentials_set(session_, GNUTLS_CRD_CERTIFICATE, NodeConfig::instance()->security()->cred_));
    if (NodeConfig::instance()->security()->hasCiphers())
        gnuTlsCheckSuccess(gnutls_priority_set(session_, NodeConfig::instance()->security()->prio_));
    else
        gnuTlsCheckSuccess(gnutls_set_default_priority(session_));
    gnutls_handshake_set_post_client_hello_function(session_, onClientHello);
    HttpSocket::initTransport();

    SecurityMaster::instance()->prepareSessionResumption(session_);
}

class ClientHelloContext: public Object
{
public:
    static ClientHelloContext *instance() { return ThreadLocalSingleton<ClientHelloContext>::instance(); }

    void selectService(gnutls_session_t session)
    {
        try {
            size_t size = 0;
            unsigned type = 0;
            int ret = gnutls_server_name_get(session, NULL, &size, &type, 0);
            if (type == GNUTLS_NAME_DNS) {
                CC_ASSERT(ret == GNUTLS_E_SHORT_MEMORY_BUFFER);
                serverName_ = String(size);
                ret = gnutls_server_name_get(session, mutate(serverName_)->bytes(), &size, &type, 0);
                if (ret != GNUTLS_E_SUCCESS) {
                    CCNODE_ERROR() << peerAddress_ << ": " << gnutls_strerror(ret) << nl;
                    serverName_ = "";
                }
                if (serverName_->count() > 0) {
                    if (serverName_->at(serverName_->count() - 1) == 0)
                        mutate(serverName_)->truncate(serverName_->count() - 1);
                    if (ErrorLog::instance()->infoStream() != NullStream::instance())
                        CCNODE_INFO() << "TLS client hello: SNI=\"" << serverName_ << "\"" << nl;
                    serviceInstance_ = NodeConfig::instance()->selectService(serverName_);
                    if (serviceInstance_) {
                        if (serviceInstance_->security()->hasCredentials())
                            HttpServerSocket::gnuTlsCheckSuccess(gnutls_credentials_set(session, GNUTLS_CRD_CERTIFICATE, serviceInstance_->security()->cred_), peerAddress_);
                        if (serviceInstance_->security()->hasCiphers())
                            HttpServerSocket::gnuTlsCheckSuccess(gnutls_priority_set(session, serviceInstance_->security()->prio_), peerAddress_);
                    }
                }
            }
        }
        catch (Exception &ex) {
            CCNODE_ERROR() << peerAddress_ << ": " << ex << nl;
        }
    }

    void reset(const SocketAddress *peerAddress)
    {
        peerAddress_ = peerAddress;
        serverName_ = "";
        serviceInstance_ = 0;
    }

    String serverName() const { return serverName_; }
    ServiceInstance *serviceInstance() const { return serviceInstance_; }

private:
    friend class ThreadLocalSingleton<ClientHelloContext>;

    ClientHelloContext(): serviceInstance_(0) {}

    Ref<const SocketAddress> peerAddress_;
    String serverName_;
    ServiceInstance *serviceInstance_;
};

int HttpServerSocket::onClientHello(gnutls_session_t session)
{
    ClientHelloContext::instance()->selectService(session);
    return 0;
}

ServiceInstance *HttpServerSocket::handshake()
{
    CC_ASSERT(mode_ & Connected);

    if (!(mode_ & Secure)) return 0;

    CC_ASSERT(!(mode_ & Open));

    ClientHelloContext::instance()->reset(address());

    while (true) {
        int ret = gnutls_handshake(session_);
        if (ret == GNUTLS_E_SUCCESS) break;
        if (ret < 0) {
            if (gnutls_error_is_fatal(ret))
                gnuTlsCheckSuccess(ret);
        }
    }

    mode_ |= Open;

    if (ErrorLog::instance()->infoStream() != NullStream::instance()) {
        double t1 = System::now();
        CCNODE_INFO() << "TLS handshake took " << int(1000 * (t1 - t0_)) << "ms" << nl;
    }

    ServiceInstance *serviceInstance = ClientHelloContext::instance()->serviceInstance();
    if (!serviceInstance) {
        if (NodeConfig::instance()->serviceInstances()->count() == 1)
            serviceInstance = NodeConfig::instance()->serviceInstances()->at(0);
        if (ClientHelloContext::instance()->serverName() == "")
            serviceInstance = NodeConfig::instance()->selectService("");
    }

    return serviceInstance;
}

void HttpServerSocket::upgradeToSecureTransport()
{
    CC_ASSERT(mode_ & Connected);
    CC_ASSERT(!(mode_ & Secure));

    mode_ = mode_ & ~Open;
    mode_ |= Secure;
    initSession();
}

bool HttpServerSocket::waitInput()
{
    double d = te_ - System::now();
    if (d <= 0) throw RequestTimeout();
    return waitFor(IoReady::Read, d * 1000);
}

void HttpServerSocket::ioException(Exception &ex) const
{
    CCNODE_ERROR() << "!" << ex << nl;
}

} // namespace ccnode
