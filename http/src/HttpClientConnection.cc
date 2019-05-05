/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/LineSource>
#include <cc/http/HttpClientConnection>

namespace cc {
namespace http {

Ref<HttpClientConnection> HttpClientConnection::open(HttpClientSocket *socket)
{
    return new HttpClientConnection{socket};
}

HttpClientConnection::HttpClientConnection(HttpClientSocket *socket):
    HttpConnection{socket},
    socket_{socket}
{}

bool HttpClientConnection::isSecure() const
{
    return socket_->isSecure();
}

Ref<HttpResponse> HttpClientConnection::readResponse()
{
    Ref<HttpResponse> response = HttpResponse::create();
    readMessage(response);
    return response;
}

void HttpClientConnection::readFirstLine(LineSource *source, HttpMessage *message)
{
    String line;
    if (!source->read(&line)) throw CloseRequest{};

    HttpResponse *response = Object::cast<HttpResponse *>(message);

    Ref<StringList> parts = line->split(' ');
    if (parts->count() > 0) {
        response->version_ = parts->popFront();
        mutate(response->version_)->upcaseInsitu();
    }

    if (response->version_ != "HTTP/1.1") throw UnsupportedVersion{};

    if (parts->count() > 0)
        response->statusCode_ = parts->popFront()->toNumber<int>();

    if (parts->count() > 0)
        response->reasonPhrase_ = parts->join(" ");
}

}} // namespace cc::http
