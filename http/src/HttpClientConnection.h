/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/http/HttpConnection>
#include <cc/http/HttpClientSocket>
#include <cc/http/HttpResponse>

namespace cc {
namespace http {

class HttpClientConnection: public HttpConnection
{
public:
    static Ref<HttpClientConnection> open(HttpClientSocket *socket);

private:
    HttpClientConnection(HttpClientSocket *socket);

    virtual void readFirstLine(LineSource *source, HttpMessage *message) override;

    Ref<HttpClientSocket> socket_;
};

}} // namespace cc::http
