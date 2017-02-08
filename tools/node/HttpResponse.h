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

namespace cc { class TransferMeter; }

namespace ccnode {

using namespace cc;

class HttpClientConnection;
class ServiceWorker;

class HttpResponse: public Object
{
public:
    static Ref<HttpResponse> create(HttpClientConnection *client);

    void setStatus(int statusCode, String reasonPhrase = "");
    void setHeader(String name, String value);
    typedef Map<String, String> Header;
    void setHeader(Header *header);
    void beginTransmission(ssize_t contentLength = -1);
    void write(String bytes);
    Format chunk(String pattern);
    Format chunk();
    void endTransmission();

private:
    friend class ServiceWorker;

    HttpResponse(HttpClientConnection *client);

    void writeHeader();
    Stream *payload();

    inline bool delivered() const { return headerWritten_; }
    inline int statusCode() const { return statusCode_; }
    size_t bytesWritten() const;

    Ref<HttpClientConnection> client_;
    Ref<Header> header_;
    bool headerWritten_;
    Ref<TransferMeter> payload_;
    int statusCode_;
    ssize_t contentLength_;
    size_t bytesWritten_;
    String reasonPhrase_;
};

} // namespace ccnode
