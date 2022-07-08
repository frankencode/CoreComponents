/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/HttpStatus>

namespace cc {

const char *httpReasonPhrase(HttpStatus status)
{
    const char *s = "";

    switch (status) {
        case HttpStatus::Continue:                    s = "Continue"; break;
        case HttpStatus::SwitchingProtocols:          s = "Switching Protocols"; break;
        case HttpStatus::OK:                          s = "OK"; break;
        case HttpStatus::Created:                     s = "Created"; break;
        case HttpStatus::Accepted:                    s = "Accepted"; break;
        case HttpStatus::NonAuthoritativeInformation: s = "Non-Authoritative Information"; break;
        case HttpStatus::NoContent:                   s = "No Content"; break;
        case HttpStatus::ResetContent:                s = "Reset Content"; break;
        case HttpStatus::PartialContent:              s = "Partial Content"; break;
        case HttpStatus::MultipleChoices:             s = "Multiple Choices"; break;
        case HttpStatus::MovedPermanently:            s = "Moved Permanently"; break;
        case HttpStatus::MovedTemporarily:            s = "Moved Temporarily"; break;
        case HttpStatus::SeeOther:                    s = "See Other"; break;
        case HttpStatus::NotModified:                 s = "Not Modified"; break;
        case HttpStatus::UseProxy:                    s = "Use Proxy"; break;
        case HttpStatus::BadRequest:                  s = "Bad Request"; break;
        case HttpStatus::Unauthorized:                s = "Unauthorized"; break;
        case HttpStatus::PaymentRequired:             s = "Payment Required"; break;
        case HttpStatus::Forbidden:                   s = "Forbidden"; break;
        case HttpStatus::NotFound:                    s = "Not Found"; break;
        case HttpStatus::MethodNotAllowed:            s = "Method Not Allowed"; break;
        case HttpStatus::NotAcceptable:               s = "Not Acceptable"; break;
        case HttpStatus::ProxyAuthenticationRequired: s = "Proxy Authentication Required"; break;
        case HttpStatus::RequestTimeout:              s = "Request Timeout"; break;
        case HttpStatus::Conflict:                    s = "Conflict"; break;
        case HttpStatus::Gone:                        s = "Gone"; break;
        case HttpStatus::LengthRequired:              s = "Length Required"; break;
        case HttpStatus::PreconditionFailed:          s = "Precondition Failed"; break;
        case HttpStatus::PayloadTooLarge:             s = "Request Entity Too Large"; break;
        case HttpStatus::RequestUriTooLong:           s = "Request-URI Too Long"; break;
        case HttpStatus::UnsupportedMediaType:        s = "Unsupported Media Type"; break;
        case HttpStatus::InternalServerError:         s = "Internal Server Error"; break;
        case HttpStatus::NotImplemented:              s = "Not Implemented"; break;
        case HttpStatus::BadGateway:                  s = "Bad Gateway"; break;
        case HttpStatus::ServiceUnavailable:          s = "Service Unavailable"; break;
        case HttpStatus::GatewayTimeout:              s = "Gateway Timeout"; break;
        case HttpStatus::UnsupportedVersion:          s = "HTTP Version Not Supported"; break;
    };

    return s;
}

} // namespace cc
