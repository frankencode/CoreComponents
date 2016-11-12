/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/System>
#include <cc/Date>
#include <cc/stream/TransferMeter>
#include "utils.h"
#include "NodeConfig.h"
#include "HttpClientConnection.h"
#include "ChunkedSink.h"
#include "HttpResponse.h"

namespace ccnode {

using namespace cc::stream;

Ref<HttpResponse> HttpResponse::create(HttpClientConnection *client)
{
    return new HttpResponse(client);
}

HttpResponse::HttpResponse(HttpClientConnection *client):
    client_(client),
    header_(Header::create()),
    headerWritten_(false),
    statusCode_(200),
    contentLength_(-1),
    bytesWritten_(0),
    reasonPhrase_("OK")
{}

void HttpResponse::setStatus(int statusCode, String reasonPhrase)
{
    statusCode_ = statusCode;
    reasonPhrase_ = reasonPhrase;
    if (reasonPhrase_ == "") reasonPhrase_ = reasonPhraseByStatusCode(statusCode_);
}

void HttpResponse::setHeader(String name, String value)
{
    header_->establish(name, value);
}

void HttpResponse::setHeader(Header *header)
{
    Ref<Header> headerSaved = header_;
    header_ = header;
    for (int i = 0; i < headerSaved->count(); ++i)
        header_->insert(headerSaved->keyAt(i), headerSaved->valueAt(i));
}

void HttpResponse::writeHeader()
{
    String now = formatDate(Date::breakdown(System::now()));
    {
        String nodeVersion = nodeConfig()->version();
        if (nodeVersion != "")
            header_->insert("Server", nodeVersion);
    }
    header_->insert("Date", now);

    if (statusCode_ != 304/*Not Modified*/) {
        if (contentLength_ >= 0) {
            header_->remove("Transfer-Encoding");
            header_->establish("Content-Length", str(contentLength_));
        }
        else {
            header_->establish("Transfer-Encoding", "chunked");
        }
        header_->insert("Last-Modified", now);
    }

    Format sink(client_->stream());
    sink << "HTTP/1.1 " << statusCode_ << " " << reasonPhrase_ << "\r\n";
    for (int i = 0; i < header_->count(); ++i)
        sink << header_->keyAt(i) << ":" << header_->valueAt(i) << "\r\n";
    sink << "\r\n";

    headerWritten_ = true;
}

void HttpResponse::beginTransmission(ssize_t contentLength)
{
    if (!headerWritten_) {
        contentLength_ = contentLength;
        writeHeader();
    }
}

Stream *HttpResponse::payload()
{
    if (!payload_) {
        if (!headerWritten_) writeHeader();
        Ref<Stream> stream = client_->stream();
        if (contentLength_ < 0) {
            stream = ChunkedSink::open(stream);
        }
        payload_ = TransferMeter::open(stream);
    }
    return payload_;
}

size_t HttpResponse::bytesWritten() const
{
    return (payload_) ? payload_->totalWritten() : bytesWritten_;
}

void HttpResponse::write(String bytes)
{
    payload()->write(bytes);
}

Format HttpResponse::chunk(String pattern)
{
    return Format(pattern, payload());
}

Format HttpResponse::chunk()
{
    return Format(payload());
}

void HttpResponse::endTransmission()
{
    if (payload_) {
        bytesWritten_ = payload_->totalWritten();
        payload_ = 0;
    }
}

} // namespace ccnode
