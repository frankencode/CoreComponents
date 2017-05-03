/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/http/HttpGenerator>

namespace cc {
namespace http {

class HttpRequestGenerator: public HttpGenerator
{
public:
    static Ref<HttpRequestGenerator> create(HttpConnection *server);

    void setMethod(String method);
    void setPath(String path);
    void setVersion(String version);
    void setUserAgent(String userAgent);

private:
    HttpRequestGenerator(HttpConnection *server);

    virtual void polishHeader() override;
    virtual void writeFirstLine(Format &sink) override;

    String method_;
    String path_;
    String version_;
};

}} // namespace cc::http
