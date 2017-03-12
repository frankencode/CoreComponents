/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <gnutls/gnutls.h>

namespace cc {
namespace http {

class SecuritySettings: public Object {
public:
    static Ref<SecuritySettings> create();
    static Ref<SecuritySettings> createDefault();

    String certPath() const { return certPath_; }
    String keyPath() const { return keyPath_; }
    void setCredentials(String certPath, String keyPath);

    String trustFilePath() const { return trustFilePath_; }
    void setTrustFilePath(String trustFilePath);

    void setSystemTrust();

    String ciphers() const { return ciphers_; }
    void setCiphers(String ciphers);

    void establish(gnutls_session_t session);

protected:
    SecuritySettings();
    ~SecuritySettings();

    String certPath_;
    String keyPath_;
    String trustFilePath_;
    String ciphers_;

    gnutls_certificate_credentials_t cred_;
    gnutls_priority_t prio_;
};

}} // namespace cc::http
