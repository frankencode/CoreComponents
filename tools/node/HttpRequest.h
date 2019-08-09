/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/http/HttpMessage>

namespace ccnode {

using namespace cc;
using namespace cc::http;

class HttpServerConnection;

class HttpRequest: public HttpMessage
{
public:
    static Ref<HttpRequest> create() { return new HttpRequest; }

    String method() const { return method_; }
    String uri() const { return uri_; }
    String query() const { return uri_->select(uri_->find('?') + 1, uri_->count()); }
    String version() const { return version_; }
    int majorVersion() const { return majorVersion_; }
    int minorVersion() const { return minorVersion_; }
    String host() const { return host_; }

    String line() const { return line_; }
    double time() const { return time_; }

private:
    friend class HttpServerConnection;
    HttpRequest() {}

    String method_;
    String uri_;
    String version_;
    int majorVersion_;
    int minorVersion_;
    String host_;

    String line_;
    double time_;

    Ref<Stream> payload_;
};

} // namespace ccnode
