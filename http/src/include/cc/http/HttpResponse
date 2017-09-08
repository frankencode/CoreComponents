/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/http/HttpMessage>

namespace cc {
namespace http {

class HttpClientConnection;

class HttpResponse: public HttpMessage
{
public:
    inline String version() const { return version_; }
    inline int statusCode() const { return statusCode_; }
    inline String reasonPhrase() const { return reasonPhrase_; }

private:
    friend class HttpClientConnection;
    inline static Ref<HttpResponse> create() { return new HttpResponse; }
    HttpResponse() {}

    String version_;
    int statusCode_;
    String reasonPhrase_;
};

}} // namespace cc::http
