/*
 * Copyright (C) 2007-2017 Frank Mertens.
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

namespace cc {
namespace http {

class HttpConnection;

/** \brief HTTP message generator
  */
class HttpGenerator: public Object
{
public:
    void setHeader(String name, String value);
    typedef Map<String, String> Header;
    void setHeader(Header *header);
    void beginTransmission(ssize_t contentLength = -1);
    void write(String bytes);
    Format chunk(String pattern);
    Format chunk();
    void endTransmission();

protected:
    HttpGenerator(HttpConnection *peer);
    ~HttpGenerator();

    virtual void polishHeader() = 0;
    virtual void writeFirstLine(Format &sink) = 0;

    void writeHeader();
    Stream *payload();

    Ref<HttpConnection> peer_;
    Ref<Header> header_;
    bool headerWritten_;
    Ref<TransferMeter> payload_;
    ssize_t contentLength_;
    size_t bytesWritten_;
};

}} // namespace cc::http
