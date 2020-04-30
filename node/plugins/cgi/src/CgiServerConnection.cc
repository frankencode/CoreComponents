/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/CgiServerConnection>

namespace cc {
namespace node {

Ref<CgiServerConnection> CgiServerConnection::open(Stream *stream)
{
    return new CgiServerConnection{stream};
}

CgiServerConnection::CgiServerConnection(Stream *stream):
    HttpConnection{stream}
{}

Ref<HttpMessage> CgiServerConnection::readResponse()
{
    Ref<HttpMessage> response = HttpMessage::create();
    readMessage(response);
    return response;
}

}} // namespace cc::node
