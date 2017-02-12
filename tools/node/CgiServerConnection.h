/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/http/HttpConnection>

namespace ccnode {

using namespace cc;
using namespace cc::http;

class CgiServerConnection: public HttpConnection
{
public:
    static Ref<CgiServerConnection> open(Stream *stream);

    Ref<HttpMessage> readResponse();

private:
    CgiServerConnection(Stream *stream);
};

} // namespace ccnode
