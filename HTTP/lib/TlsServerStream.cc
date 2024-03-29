/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TlsServerStream>
#include <cc/TlsServerOptionsState>
#include <cc/TlsHelloContext>
#include <cc/TlsStreamState>
#include <cassert>

namespace cc {

struct TlsServerStream::State: public TlsStream::State
{
    State(const Stream &stream, const SocketAddress &peerAddress, const TlsServerOptions &options, const TlsSessionCache &sessionCache):
        TlsStream::State{stream},
        peerAddress_{peerAddress},
        options_{options},
        sessionCache_{sessionCache}
    {
        checkTlsSuccess(gnutls_init(&session_, GNUTLS_SERVER));
        checkTlsSuccess(gnutls_set_default_priority(session_));
        options_->establish(session_, peerAddress);
        gnutls_handshake_set_post_client_hello_function(session_, onClientHello);
        initTransport();
        sessionCache_.prepareSessionResumption(session_);
    }

    ~State()
    {
        if (established_) {
            gnutls_bye(session_, GNUTLS_SHUT_WR);
        }
        gnutls_deinit(session_);
    }

    void handshake(TlsServiceSelector &&serviceSelector, int timeout)
    {
        TlsHelloContext{}.init(peerAddress_, move(serviceSelector));
        timeout_ = timeout;

        while (true) {
            int ret = gnutls_handshake(session_);
            checkIoError();
            if (ret == GNUTLS_E_SUCCESS) {
                established_ = true;
                break;
            }
            if (ret < 0) {
                if (gnutls_error_is_fatal(ret)) checkTlsSuccess(ret);
            }
        }
    }

    static int onClientHello(gnutls_session_t session)
    {
        size_t size = 0;
        unsigned type = 0;
        int ret = gnutls_server_name_get(session, NULL, &size, &type, 0);
        if (type == GNUTLS_NAME_DNS) {
            assert(ret == GNUTLS_E_SHORT_MEMORY_BUFFER);
            String serverName = String::allocate(size);
            ret = gnutls_server_name_get(session, serverName, &size, &type, 0);
            if (ret != GNUTLS_E_SUCCESS) serverName = "";
            if (serverName.count() > 0) {
                if (serverName.at(serverName.count() - 1) == 0) {
                    serverName.truncate(serverName.count() - 1);
                }
                TlsServerOptions tlsOptions = TlsHelloContext{}.selectService(serverName);
                if (tlsOptions) {
                    tlsOptions->establish(session, TlsHelloContext{}.peerAddress());
                }
            }
        }
        return 0;
    }

    Stream stream_;
    SocketAddress peerAddress_;
    TlsServerOptions options_;
    TlsSessionCache sessionCache_;
    bool established_ { false };
};

TlsServerStream::TlsServerStream(const Stream &stream, const SocketAddress &peerAddress, const TlsServerOptions &options, const TlsSessionCache &sessionCache):
    TlsStream{new State{stream, peerAddress, options, sessionCache}}
{}

void TlsServerStream::handshake(TlsServiceSelector &&serviceSelector, int timeout)
{
    me().handshake(move(serviceSelector), timeout);
}

TlsServerStream::State &TlsServerStream::me()
{
    return Object::me.as<State>();
}

const TlsServerStream::State &TlsServerStream::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
