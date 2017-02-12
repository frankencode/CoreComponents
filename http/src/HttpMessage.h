/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/String>
#include <cc/Map>
#include <cc/Stream>

namespace cc {
namespace http {

class HttpConnection;

class HttpMessage: public Map<String, String>
{
public:
    inline static Ref<HttpMessage> create() { return new HttpMessage; }
    inline Stream *payload() const { return payload_; }

protected:
    friend class HttpConnection;
    HttpMessage() {}

    Ref<Stream> payload_;
};

}} // namespace cc::http
