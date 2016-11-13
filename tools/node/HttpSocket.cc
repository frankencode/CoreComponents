/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <stddef.h>
#include <poll.h>
#include <unistd.h>
#include <cc/assert>
#include <cc/ThreadLocalSingleton>
#include <cc/System>
#include <cc/NullStream>
#include "exceptions.h"
#include "ErrorLog.h"
#include "NodeConfig.h"
#include "SecurityConfig.h"
#include "SecurityMaster.h"
#include "HttpSocket.h"

namespace ccnode {

Ref<HttpSocket> HttpSocket::listen(const SocketAddress *address, int backlog)
{
    Ref<HttpSocket> socket =
        new HttpSocket(
            address,
            (address->port() % 80 == 0) ? Plaintext : Secure
        );

    socket->StreamSocket::listen(backlog);

    return socket;
}

Ref<HttpSocket> HttpSocket::accept()
{
    CC_ASSERT(!(mode_ & Connected));

    Ref<HttpSocket> client = new HttpSocket(SocketAddress::create(address_->family()), mode_);
    client->fd_ = StreamSocket::accept(client->address_);
    client->connected_ = true;
    client->mode_ |= Connected | ((mode_ & Plaintext) ? Open : 0);
    client->sessionInit();

    return client;
}

HttpSocket::HttpSocket(const SocketAddress *address, int mode):
    StreamSocket(address),
    mode_(mode)
{}

HttpSocket::~HttpSocket()
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

void HttpSocket::sessionInit()
{
    CC_ASSERT(mode_ & Connected);

    t0_ = System::now();
    te_ = t0_ + nodeConfig()->connectionTimeout();

    if (mode_ & Plaintext) return;

    CC_ASSERT(!(mode_ & Open));

    gnuTlsCheckSuccess(gnutls_init(&session_, GNUTLS_SERVER));
    if (nodeConfig()->security()->hasCredentials())
        gnuTlsCheckSuccess(gnutls_credentials_set(session_, GNUTLS_CRD_CERTIFICATE, nodeConfig()->security()->cred_));
    if (nodeConfig()->security()->hasCiphers())
        gnuTlsCheckSuccess(gnutls_priority_set(session_, nodeConfig()->security()->prio_));
    else
        gnuTlsCheckSuccess(gnutls_set_default_priority(session_));
    gnutls_handshake_set_post_client_hello_function(session_, onClientHello);
    gnutls_transport_set_ptr(session_, this);
    gnutls_transport_set_pull_function(session_, gnuTlsPull);
    gnutls_transport_set_vec_push_function(session_, gnuTlsPushVec);

    SecurityMaster::instance()->prepareSessionResumption(session_);
}

class ClientHelloContext: public Object
{
public:
    static ClientHelloContext *instance() { return ThreadLocalSingleton<ClientHelloContext>::instance(); }

    void selectService(gnutls_session_t session)
    {
        size_t size = 0;
        unsigned type = 0;
        int ret = gnutls_server_name_get(session, NULL, &size, &type, 0);
        if (type == GNUTLS_NAME_DNS) {
            CC_ASSERT(ret == GNUTLS_E_SHORT_MEMORY_BUFFER);
            serverName_ = String(size);
            ret = gnutls_server_name_get(session, serverName_->bytes(), &size, &type, 0);
            if (ret != GNUTLS_E_SUCCESS) {
                CCNODE_ERROR() << peerAddress_ << ": " << gnutls_strerror(ret) << nl;
                serverName_ = "";
            }
            if (serverName_->count() > 0) {
                if (serverName_->at(serverName_->count() - 1) == 0)
                    serverName_->truncate(serverName_->count() - 1);
                if (errorLog()->infoStream() != NullStream::instance())
                    CCNODE_INFO() << "TLS client hello: SNI=\"" << serverName_ << "\"" << nl;
                serviceInstance_ = nodeConfig()->selectService(serverName_);
                if (serviceInstance_) {
                    if (serviceInstance_->security()->hasCredentials())
                        HttpSocket::gnuTlsCheckSuccess(gnutls_credentials_set(session, GNUTLS_CRD_CERTIFICATE, serviceInstance_->security()->cred_), peerAddress_);
                    if (serviceInstance_->security()->hasCiphers())
                        HttpSocket::gnuTlsCheckSuccess(gnutls_priority_set(session, serviceInstance_->security()->prio_), peerAddress_);
                }
            }
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

int HttpSocket::onClientHello(gnutls_session_t session)
{
    ClientHelloContext::instance()->selectService(session);
    return 0;
}

ServiceInstance *HttpSocket::handshake()
{
    CC_ASSERT(mode_ & Connected);

    if (mode_ & Plaintext) return 0;

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

    if (errorLog()->infoStream() != NullStream::instance()) {
        double t1 = System::now();
        CCNODE_INFO() << "TLS handshake took " << int(1000 * (t1 - t0_)) << "ms" << nl;
    }

    ServiceInstance *serviceInstance = ClientHelloContext::instance()->serviceInstance();
    if (!serviceInstance) {
        if (nodeConfig()->serviceInstances()->count() == 1)
            serviceInstance = nodeConfig()->serviceInstances()->at(0);
        if (ClientHelloContext::instance()->serverName() == "")
            serviceInstance = nodeConfig()->selectService("");
    }

    return serviceInstance;
}

void HttpSocket::upgradeToSecureTransport()
{
    CC_ASSERT(mode_ & Connected);
    CC_ASSERT(mode_ & Plaintext);

    mode_ = mode_ & ~(Open|Plaintext);
    mode_ |= Secure;
    sessionInit();
}

int HttpSocket::read(ByteArray *data)
{
    if (data->count() == 0) return 0;

    if (mode_ & Plaintext) {
        waitInput();
        return StreamSocket::read(data);
    }

    int ret = gnutls_record_recv(session_, data->bytes(), data->count());
    gnuTlsCheckError(ret);
    CC_ASSERT(ret >= 0);

    return ret;
}

void HttpSocket::write(const ByteArray *data)
{
    if (data->count() == 0) return;

    if (mode_ & Plaintext) {
        StreamSocket::write(data);
        return;
    }

    String pending = data;
    while (true) {
        int ret = gnutls_record_send(session_, pending->bytes(), pending->count());
        if (ret == pending->count()) break;
        gnuTlsCheckError(ret);
        CC_ASSERT(ret > 0);
        if (ret < pending->count())
            pending = pending->select(ret, pending->count());
    }
}

void HttpSocket::write(const StringList *parts)
{
    if (mode_ & Plaintext)
        StreamSocket::write(parts);
    else
        write(parts->join());
}

void HttpSocket::waitInput()
{
    double d = te_ - System::now();
    if (d <= 0) throw RequestTimeout();

    if (!SystemIo::poll(fd_, SystemIo::ReadyRead, d * 1000))
        throw RequestTimeout();
}

bool HttpSocket::gnuTlsCheckSuccess(int ret)
{
    return gnuTlsCheckSuccess(ret, address());
}

void HttpSocket::gnuTlsCheckError(int ret)
{
    gnuTlsCheckError(ret, address());
}

bool HttpSocket::gnuTlsCheckSuccess(int ret, const SocketAddress *peerAddress)
{
    if (ret != GNUTLS_E_SUCCESS) {
        CCNODE_ERROR() << peerAddress << ": " << gnutls_strerror(ret) << nl;
        throw InternalServerError();
    }
    return true;
}

void HttpSocket::gnuTlsCheckError(int ret, const SocketAddress *peerAddress)
{
    if (ret < 0) {
        CCNODE_ERROR() << peerAddress << ": " << gnutls_strerror(ret) << nl;
        throw InternalServerError();
    }
}

ssize_t HttpSocket::gnuTlsPull(gnutls_transport_ptr_t ctx, void *data, size_t size)
{
    ssize_t n = -1;
    try {
        HttpSocket *socket = (HttpSocket *)ctx;
        socket->waitInput();
        n = SystemIo::read(socket->fd(), data, size);
    }
    catch (Exception &ex) {
        CCNODE_ERROR() << ex << nl;
    }

    return n;
}

ssize_t HttpSocket::gnuTlsPushVec(gnutls_transport_ptr_t ctx, const giovec_t *iov, int iovcnt)
{
    CC_STATIC_ASSERT(offsetof(struct iovec, iov_base) == offsetof(giovec_t, iov_base));
    CC_STATIC_ASSERT(offsetof(struct iovec, iov_len) == offsetof(giovec_t, iov_len));
    CC_STATIC_ASSERT(sizeof(struct iovec) == sizeof(giovec_t));

    HttpSocket *socket = (HttpSocket *)ctx;
    try {
        SystemIo::writev(socket->fd(), (const struct iovec *)iov, iovcnt);
    }
    catch (Exception &ex) {
        CCNODE_ERROR() << ex << nl;
        return -1;
    }

    ssize_t total = 0;
    for (int i = 0; i < iovcnt; ++i)
        total += iov[i].iov_len;
    return total;
}

} // namespace ccnode
