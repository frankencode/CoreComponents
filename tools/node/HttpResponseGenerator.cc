/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Format>
#include <cc/System>
#include <cc/Date>
#include <cc/TransferMeter>
#include <cc/http/utils>
#include <cc/http/HttpConnection>
#include "HttpResponseGenerator.h"

namespace ccnode {

using namespace cc::http;

Ref<HttpResponseGenerator> HttpResponseGenerator::create(HttpConnection *client)
{
    return new HttpResponseGenerator{client};
}

HttpResponseGenerator::HttpResponseGenerator(HttpConnection *client):
    HttpGenerator{client},
    statusCode_{200},
    reasonPhrase_{"OK"}
{}

void HttpResponseGenerator::setStatus(int statusCode, const String &reasonPhrase)
{
    statusCode_ = statusCode;
    reasonPhrase_ = reasonPhrase;
    if (reasonPhrase_ == "") reasonPhrase_ = reasonPhraseByStatusCode(statusCode_);
}

void HttpResponseGenerator::setNodeVersion(const String &nodeVersion)
{
    nodeVersion_ = nodeVersion;
}

size_t HttpResponseGenerator::bytesWritten() const
{
    return (payload_) ? payload_->totalWritten() : bytesWritten_;
}

void HttpResponseGenerator::polishHeader()
{
    String now = formatDate(Date::breakdown(System::now()));
    if (nodeVersion_ != "") header_->insert("Server", nodeVersion_);
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
}

void HttpResponseGenerator::writeFirstLine(Format &sink)
{
    sink << "HTTP/1.1 " << statusCode_ << " " << reasonPhrase_ << "\r\n";
}

} // namespace ccnode
