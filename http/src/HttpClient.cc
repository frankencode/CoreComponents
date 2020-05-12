/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/http/HttpClient>
#include <cc/http/HttpClientSocket>
#include <cc/http/HttpClientConnection>

namespace cc {
namespace http {

Ref<HttpClient> HttpClient::create(const HttpClientSecurity *security)
{
    return new HttpClient{security};
}

HttpClient::HttpClient(const HttpClientSecurity *security)
{}

}} // namespace cc::http
