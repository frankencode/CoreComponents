/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include "HttpMessage.h"

namespace ccnode {

using namespace cc;

class HttpClientConnection;

class HttpRequest: public HttpMessage
{
public:
    inline String method() const { return method_; }
    inline String uri() const { return uri_; }
    inline String query() const { return uri_->select(uri_->find('?') + 1, uri_->count()); }
    inline String version() const { return version_; }
    inline int majorVersion() const { return majorVersion_; }
    inline int minorVersion() const { return minorVersion_; }
    inline String host() const { return host_; }

    inline String line() const { return line_; }
    inline double time() const { return time_; }

private:
    friend class HttpClientConnection;
    inline static Ref<HttpRequest> create() { return new HttpRequest; }
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
