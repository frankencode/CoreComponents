/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/http/HttpResponse>
#include <cc/http/HttpRequestGenerator>

namespace cc {
namespace http {

class HttpClient: public Object
{
public:
/*    HttpRequestGenerator *request() const;
    bool readResponse(Ref<HttpResponse> *response);
    void shutdown();

    WIP...
    */

private:
    Ref<HttpClientConnection> connection_;
};

}} // namespace cc::http
