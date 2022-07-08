/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TlsCertificateError>
#include <gnutls/gnutls.h>

namespace cc {

TlsCertificateError::TlsCertificateError(unsigned status):
    status_{status}
{}

String TlsCertificateError::message() const
{
    String s;
    gnutls_datum_t datum;
    if (gnutls_certificate_verification_status_print(status_, GNUTLS_CRT_X509, &datum, 0) == GNUTLS_E_SUCCESS) {
        s = reinterpret_cast<const char *>(datum.data);
        gnutls_free(datum.data);
    }
    return s;
}

} // namespace cc
