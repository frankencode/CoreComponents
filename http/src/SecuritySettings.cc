/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/http/exceptions>
#include <cc/http/SecuritySettings>

namespace cc {
namespace http {

SecuritySettings::SecuritySettings():
    cred_(0),
    prio_(0)
{
    gnutls_certificate_allocate_credentials(&cred_);
}

SecuritySettings::~SecuritySettings()
{
    if (prio_) gnutls_priority_deinit(prio_);
    gnutls_certificate_free_credentials(cred_);
}

void SecuritySettings::setCredentials(String certPath, String keyPath)
{
    certPath_ = certPath;
    keyPath_ = keyPath;
    int ret = gnutls_certificate_set_x509_key_file(cred_, certPath, keyPath, GNUTLS_X509_FMT_PEM);
    if (ret != GNUTLS_E_SUCCESS) throw TlsError(ret);
}

void SecuritySettings::setTrustFilePath(String trustFilePath)
{
    trustFilePath_ = trustFilePath;
    int ret = gnutls_certificate_set_x509_trust_file(cred_, trustFilePath_, GNUTLS_X509_FMT_PEM);
    if (ret != GNUTLS_E_SUCCESS) throw TlsError(ret);
}

void SecuritySettings::setCiphers(String ciphers)
{
    ciphers_ = ciphers;
    if (prio_) gnutls_priority_deinit(prio_);
    int ret = gnutls_priority_init(&prio_, ciphers_, NULL);
    if (ret != GNUTLS_E_SUCCESS) throw TlsError(ret);
}

void SecuritySettings::establish(gnutls_session_t session)
{
    int ret = gnutls_credentials_set(session, GNUTLS_CRD_CERTIFICATE, cred_);
    if (ret != GNUTLS_E_SUCCESS) throw TlsError(ret);
    if (prio_) ret = gnutls_priority_set(session, prio_);
    else ret = gnutls_set_default_priority(session);
    if (ret != GNUTLS_E_SUCCESS) throw TlsError(ret);
}

}} // namespace cc::http
