/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/exceptions>
#include <cc/Format>
#include "ErrorLog.h"
#include "SecurityConfig.h"

namespace ccnode {

SecurityConfig::SecurityConfig(MetaObject *config):
    hasCredentials_{false},
    hasCiphers_{false},
    cred_{0},
    prio_{0},
    sessionResumptionKeyRefresh_{config->value("session-resumption-key-refresh")}
{
    String certPath = config->value("certificate");
    String keyPath = config->value("private-key");
    String ciphers = config->value("ciphers");

    if (ciphers == "") ciphers = "PERFORMANCE:%SERVER_PRECEDENCE";
    else hasCiphers_ = true;

    int ret = gnutls_priority_init(&prio_, ciphers, NULL);
    if (ret != GNUTLS_E_SUCCESS) {
        CCNODE_ERROR() << gnutls_strerror(ret) << nl;
        throw UsageError(gnutls_strerror(ret));
    }

    if (certPath == "" || keyPath == "") return;
    hasCredentials_ = true;

    // FIXME: error handling if certificate or private-key is empty
    // FIXME: allow session-resumption-key-refresh only at node level
    // TODO: allow MetaObject to hold a locator map for its members (MetaContext!-)

    ret = gnutls_certificate_allocate_credentials(&cred_);
    if (ret != GNUTLS_E_SUCCESS) {
        CCNODE_ERROR() << gnutls_strerror(ret) << nl;
        throw UsageError(gnutls_strerror(ret));
    }

    ret = gnutls_certificate_set_x509_key_file(cred_, certPath, keyPath, GNUTLS_X509_FMT_PEM);
    if (ret != GNUTLS_E_SUCCESS) {
        CCNODE_ERROR() << gnutls_strerror(ret) << nl;
        throw UsageError(gnutls_strerror(ret));
    }

    // FIXME: requires gnutls >=3.4
    // logCertificateInfo();
}

SecurityConfig::~SecurityConfig()
{
    if (cred_) gnutls_certificate_free_credentials(cred_);
    if (prio_) gnutls_priority_deinit(prio_);
}

#if 0
void SecurityConfig::logCertificateInfo()
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

void SecurityConfig::logCertificateInfo(gnutls_x509_crt_t cert)
{
    CCNODE_INFO() << certificateName(cert) << nl;
}

String SecurityConfig::certificateName(gnutls_x509_crt_t cert)
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

} // namespace ccnode
