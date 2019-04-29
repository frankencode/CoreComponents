/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "CgiServerConnection.h"

namespace ccnode {

Ref<CgiServerConnection> CgiServerConnection::open(Stream *stream)
{
    return new CgiServerConnection(stream);
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

} // namespace ccnode
