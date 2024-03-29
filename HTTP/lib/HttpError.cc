/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/HttpError>

namespace cc {

HttpBadRequest::HttpBadRequest():
    HttpError{HttpStatus::BadRequest, "Bad Request"}
{}

HttpForbidden::HttpForbidden():
    HttpError{HttpStatus::Forbidden, "Forbidden"}
{}

HttpNotFound::HttpNotFound():
    HttpError{HttpStatus::NotFound, "Not Found"}
{}

HttpRequestTimeout::HttpRequestTimeout():
    HttpError{HttpStatus::RequestTimeout, "Request Timeout"}
{}

HttpPayloadTooLarge::HttpPayloadTooLarge():
    HttpError{HttpStatus::PayloadTooLarge, "Payload Too Large"}
{}

HttpInternalServerError::HttpInternalServerError():
    HttpError{HttpStatus::InternalServerError, "Internal Server Error"}
{}

HttpUnsupportedVersion::HttpUnsupportedVersion():
    HttpError{HttpStatus::UnsupportedVersion, "HTTP Version Not Supported"}
{}

HttpNotImplemented::HttpNotImplemented():
    HttpError{HttpStatus::NotImplemented, "Not Implemented"}
{}

} // namespace cc
