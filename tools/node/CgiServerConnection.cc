/*
 * Copyright (C) 2007-2016 Frank Mertens.
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
    HttpConnection(stream)
{}

Ref<CgiResponse> CgiServerConnection::readResponse()
{
    Ref<CgiResponse> response = CgiResponse::create();
    readMessage(response);
    return response;
}

} // namespace ccnode
