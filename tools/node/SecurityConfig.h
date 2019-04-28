/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <gnutls/gnutls.h>
#include <cc/meta/MetaObject>

namespace ccnode {

using namespace cc;
using namespace cc::meta;

class HttpServerSocket;

class SecurityConfig: public Object
{
public:
    static Ref<SecurityConfig> load(MetaObject *config) {
        return new SecurityConfig(config);
    }

    bool hasCredentials() const { return hasCredentials_; }
    bool hasCiphers() const { return hasCiphers_; }
    double sessionResumptionKeyRefresh() const { return sessionResumptionKeyRefresh_; }

private:
    friend class HttpServerSocket;
    friend class ClientHelloContext;

    SecurityConfig(MetaObject *config);
    ~SecurityConfig();

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

} // namespace ccnode
