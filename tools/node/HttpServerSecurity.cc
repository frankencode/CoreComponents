/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "HttpServerSecurity.h"
#include <cc/http/exceptions>

namespace cc {
namespace http {

Ref<HttpServerSecurity> HttpServerSecurity::load(const MetaObject *config)
{
    return new HttpServerSecurity{config};
}

HttpServerSecurity::HttpServerSecurity(const MetaObject *config):
    hasCredentials_{false},
    hasCiphers_{false},
    cred_{nullptr},
    prio_{nullptr},
    sessionResumptionKeyRefresh_{config->value("session-resumption-key-refresh")}
{
    String certPath = config->value("certificate");
    String keyPath = config->value("private-key");
    String ciphers = config->value("ciphers");

    if (ciphers == "") ciphers = "PERFORMANCE:%SERVER_PRECEDENCE";
    else hasCiphers_ = true;

    int ret = gnutls_priority_init(&prio_, ciphers, NULL);
    if (ret != GNUTLS_E_SUCCESS) throw SecurityError{ret};

    if (certPath == "" || keyPath == "") return;
    hasCredentials_ = true;

    // FIXME: error handling if certificate or private-key is empty
    // FIXME: allow session-resumption-key-refresh only at node level
    // TODO: allow MetaObject to hold a locator map for its members (MetaContext!-)

    ret = gnutls_certificate_allocate_credentials(&cred_);
    if (ret != GNUTLS_E_SUCCESS) throw SecurityError{ret};

    ret = gnutls_certificate_set_x509_key_file(cred_, certPath, keyPath, GNUTLS_X509_FMT_PEM);
    if (ret != GNUTLS_E_SUCCESS) throw SecurityError{ret};

    // FIXME: requires gnutls >=3.4
    // logCertificateInfo();
}

HttpServerSecurity::~HttpServerSecurity()
{
    if (cred_) gnutls_certificate_free_credentials(cred_);
    if (prio_) gnutls_priority_deinit(prio_);
}

void HttpServerSecurity::establish(gnutls_session_t session, const SocketAddress *peerAddress) const
{
    if (hasCredentials()) {
        int ret = gnutls_credentials_set(session, GNUTLS_CRD_CERTIFICATE, cred_);
        if (ret != GNUTLS_E_SUCCESS) throw SecurityError{ret, peerAddress};
    }
    if (hasCiphers()) {
        int ret = gnutls_priority_set(session, prio_);
        if (ret != GNUTLS_E_SUCCESS) throw SecurityError{ret, peerAddress};
    }
}

#if 0
void HttpServerSecurity::logCertificateInfo()
{
    gnutls_x509_crt_t *certs = 0;
    unsigned certsCount = 0;
    int ret = gnutls_certificate_get_x509_crt(cred_, 0, &certs, &certsCount);
    if (ret != GNUTLS_E_SUCCESS) return;
    for (int i = 0; i < certsCount; ++i)
        logCertificateInfo(certs[i]);
    for (int i = 0; i < certsCount; ++i)
        gnutls_x509_cert_deinit(certs[i]);
    gnutls_free(certs);
}

void HttpServerSecurity::logCertificateInfo(gnutls_x509_crt_t cert)
{
    CCNODE_INFO() << certificateName(cert) << nl;
}

String HttpServerSecurity::certificateName(gnutls_x509_crt_t cert)
{
    size_t size = 0;
    int ret = gnutls_x509_crt_get_dn(cert, 0, &size);
    if (ret != GNUTLS_E_SHORT_MEMORY_BUFFER) return;
    String buf(size);
    ret = gnutls_x509_crt_get_dn(cert, buf, &size);
    if (ret != GNUTLS_E_SUCCESS) return;
    return buf;
}
#endif

}} // namespace cc::http
