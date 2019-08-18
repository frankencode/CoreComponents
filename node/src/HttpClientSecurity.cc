/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/exceptions>
#include <cc/node/HttpClientSecurity>

namespace cc {
namespace node {

Ref<HttpClientSecurity> HttpClientSecurity::create()
{
    return new HttpClientSecurity;
}

Ref<HttpClientSecurity> HttpClientSecurity::createDefault()
{
    Ref<HttpClientSecurity> settings = new HttpClientSecurity;
    settings->setSystemTrust();
    return settings;
}

HttpClientSecurity::HttpClientSecurity():
    cred_{nullptr},
    prio_{nullptr}
{
    gnutls_certificate_allocate_credentials(&cred_);
}

HttpClientSecurity::~HttpClientSecurity()
{
    if (prio_) gnutls_priority_deinit(prio_);
    gnutls_certificate_free_credentials(cred_);
}

void HttpClientSecurity::setCredentials(const String &certPath, const String &keyPath)
{
    certPath_ = certPath;
    keyPath_ = keyPath;
    int ret = gnutls_certificate_set_x509_key_file(cred_, certPath, keyPath, GNUTLS_X509_FMT_PEM);
    if (ret != GNUTLS_E_SUCCESS) throw SecurityError{ret};
}

void HttpClientSecurity::setTrustFilePath(const String &trustFilePath)
{
    trustFilePath_ = trustFilePath;
    int ret = gnutls_certificate_set_x509_trust_file(cred_, trustFilePath_, GNUTLS_X509_FMT_PEM);
    if (ret != GNUTLS_E_SUCCESS) throw SecurityError{ret};
}

void HttpClientSecurity::setSystemTrust()
{
    if (trustFilePath_ != "") trustFilePath_ = "";
    int ret = gnutls_certificate_set_x509_system_trust(cred_);
    if (ret < 0) throw SecurityError{ret};
}

void HttpClientSecurity::setCiphers(const String &ciphers)
{
    ciphers_ = ciphers;
    if (prio_) gnutls_priority_deinit(prio_);
    int ret = gnutls_priority_init(&prio_, ciphers_, NULL);
    if (ret != GNUTLS_E_SUCCESS) throw SecurityError{ret};
}

void HttpClientSecurity::establish(gnutls_session_t session) const
{
    int ret = gnutls_credentials_set(session, GNUTLS_CRD_CERTIFICATE, cred_);
    if (ret != GNUTLS_E_SUCCESS) throw SecurityError{ret};
    if (prio_) ret = gnutls_priority_set(session, prio_);
    else ret = gnutls_set_default_priority(session);
    if (ret != GNUTLS_E_SUCCESS) throw SecurityError{ret};
}

}} // namespace cc::node
