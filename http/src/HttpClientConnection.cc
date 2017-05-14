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

HttpClientConnection::HttpClientConnection(HttpClientSocket *socket):
    HttpConnection(socket),
    socket_(socket)
{}

void HttpClientConnection::readFirstLine(LineSource *source, HttpMessage *message)
{
    String line;
    if (!source->read(&line)) throw CloseRequest();

    HttpResponse *response = Object::cast<HttpResponse *>(message);

    Ref<StringList> parts = line->simplify()->split(' ');
    if (parts->count() > 0) {
        response->version_ = parts->popFront();
        response->version_->upcaseInsitu();
    }

    if (response->version_ != "HTTP/1.1") throw UnsupportedVersion();

    if (parts->count() > 1)
        response->statusCode_ = parts->popFront()->toNumber<int>();

    if (parts->count() > 2)
        response->reasonPhrase_ = parts->join();
}

}} // namespace cc::http
