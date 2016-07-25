/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include "HttpConnection.h"

namespace ccnode {

typedef HttpMessage CgiResponse;

class CgiServerConnection: public HttpConnection
{
public:
    static Ref<CgiServerConnection> open(Stream *stream);

    Ref<CgiResponse> readResponse();

private:
    CgiServerConnection(Stream *stream);
};

} // namespace ccnode
