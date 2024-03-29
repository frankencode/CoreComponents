/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TlsClientStream>
#include <cc/TlsClientOptionsState>
#include <cc/TlsStreamState>
#include <sys/uio.h> // iovec
#include <cassert>

namespace cc {

struct TlsClientStream::State: public TlsStream::State
{
    State(const Stream &stream, const TlsClientOptions &options):
        TlsStream::State{stream},
        options_{options}
    {
        checkTlsSuccess(gnutls_init(&session_, GNUTLS_CLIENT));

        options_->establish(session_);

        if (options_.serverName() != "") {
            checkTlsSuccess(gnutls_server_name_set(session_, GNUTLS_NAME_DNS, options_.serverName().chars(), options_.serverName().count()));
        }

        initTransport();
    }

    ~State()
    {
        if (established_) gnutls_bye(session_, GNUTLS_SHUT_WR);
        gnutls_deinit(session_);
    }

    void handshake()
    {
        while (true) {
            int ret = gnutls_handshake(session_);
            checkIoError();
            if (ret == GNUTLS_E_SUCCESS) {
                established_ = true;
                break;
            }
            if (ret < 0) {
                if (gnutls_error_is_fatal(ret))
                    throw TlsError{ret};
                else
                    tlsWarning(ret);
            }
        }

        unsigned status = 0;
        const char *hostName = nullptr;
        if (options_.serverName() != "" && options_.serverName() != "127.0.0.1") {
            hostName = options_.serverName();
        }
        checkTlsSuccess(gnutls_certificate_verify_peers3(session_, hostName, &status));
        if (status != 0) throw TlsCertificateError{status};
    }

    bool waitEstablished(int timeout) override
    {
        if (stream_.waitEstablished(timeout)) {
            timeout_ = timeout;
            handshake();
            return true;
        }
        return false;
    }

    void tlsWarning(int errorCode)
    {
        if (tlsWarning_) tlsWarning_(gnutls_strerror_name(errorCode));
    }

    TlsClientOptions options_;
    Function<void(const char *errorName)> tlsWarning_;
    bool established_ { false };
};

TlsClientStream::TlsClientStream(const Stream &stream, const TlsClientOptions &options):
    TlsStream{new State{stream, options}}
{}

void TlsClientStream::onTlsWarning(Function<void(const char *errorName)> &&f)
{
    me().tlsWarning_ = f;
}

TlsClientStream::State &TlsClientStream::me()
{
    return Object::me.as<State>();
}

} // namespace cc
