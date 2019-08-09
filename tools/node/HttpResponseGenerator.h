/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/http/HttpGenerator>

namespace ccnode {

using namespace cc;
using namespace cc::http;

class ServiceWorker;

class HttpResponseGenerator: public HttpGenerator
{
public:
    static Ref<HttpResponseGenerator> create(HttpConnection *client);

    void setStatus(int statusCode, const String &reasonPhrase = "");
    void setNodeVersion(const String &nodeVersion);

private:
    friend class ServiceWorker;

    HttpResponseGenerator(HttpConnection *client);

    Header *header() const { return header_; }
    bool delivered() const { return headerWritten_; }
    int statusCode() const { return statusCode_; }
    size_t bytesWritten() const;

    virtual void polishHeader() override;
    virtual void writeFirstLine(Format &sink) override;

    int statusCode_;
    String reasonPhrase_;
    String nodeVersion_;
};

} // namespace ccnode
