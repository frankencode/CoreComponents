/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/http/CgiServerConnection>

namespace cc {
namespace http {

Ref<CgiServerConnection> CgiServerConnection::open(const Stream &stream)
{
    return new CgiServerConnection{stream};
}

CgiServerConnection::CgiServerConnection(const Stream &stream):
    HttpConnection{stream}
{}

Ref<HttpMessage> CgiServerConnection::readResponse()
{
    Ref<HttpMessage> response = HttpMessage::create();
    readMessage(response);
    return response;
}

}} // namespace cc::http
