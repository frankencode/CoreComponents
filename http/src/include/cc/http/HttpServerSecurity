/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <gnutls/gnutls.h>
#include <cc/net/SocketAddress>
#include <cc/meta/MetaObject>

namespace cc {
namespace http {

using cc::net::SocketAddress;
using cc::meta::MetaObject;

class HttpServerSecurity: public Object
{
public:
    static Ref<HttpServerSecurity> load(const MetaObject *config);

    bool hasCredentials() const { return hasCredentials_; }
    bool hasCiphers() const { return hasCiphers_; }
    double sessionResumptionKeyRefresh() const { return sessionResumptionKeyRefresh_; }

    void establish(gnutls_session_t session, const SocketAddress *peerAddress = nullptr) const;

private:
    HttpServerSecurity(const MetaObject *config);
    ~HttpServerSecurity();

    #if 0
    void logCertificateInfo();
    void logCertificateInfo(gnutls_x509_crt_t cert);
    String certificateDn(gnutls_x509_crt_t cert);
    #endif

    bool hasCredentials_;
    bool hasCiphers_;
    gnutls_certificate_credentials_t cred_;
    gnutls_priority_t prio_;
    double sessionResumptionKeyRefresh_;
};

}} // namespace cc::http
