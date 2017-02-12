/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include "HttpGenerator.h"

namespace ccnode {

class ServiceWorker;

class HttpResponseGenerator: public HttpGenerator
{
public:
    static Ref<HttpResponseGenerator> create(HttpConnection *client);

    void setStatus(int statusCode, String reasonPhrase = "");
    void setNodeVersion(String nodeVersion);

private:
    friend class ServiceWorker;

    HttpResponseGenerator(HttpConnection *peer);

    inline Header *header() const { return header_; }
    inline int statusCode() const { return statusCode_; }
    inline bool delivered() const { return headerWritten_; }
    size_t bytesWritten() const;

    void polishHeader() override;
    void writeFirstLine(Format &sink) override;

    int statusCode_;
    String reasonPhrase_;
    String nodeVersion_;
};

} // namespace ccnode
