/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/http/HttpClient>

namespace cc {
namespace http {

Ref<HttpClient> HttpClient::create(const HttpClientSecurity *security)
{
    return new HttpClient{security};
}

}} // namespace cc::http
