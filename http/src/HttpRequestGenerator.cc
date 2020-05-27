/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/format>
#include <cc/http/HttpRequestGenerator>

namespace cc {
namespace http {

Ref<HttpRequestGenerator> HttpRequestGenerator::create(HttpConnection *server)
{
    return new HttpRequestGenerator{server};
}

HttpRequestGenerator::HttpRequestGenerator(HttpConnection *server):
    HttpGenerator{server},
    method_{"GET"},
    path_{"/"},
    version_{"HTTP/1.1"}
{}

void HttpRequestGenerator::setMethod(const string &method)
{
    method_ = method;
}

void HttpRequestGenerator::setPath(const string &path)
{
    path_ = path;
}

void HttpRequestGenerator::setVersion(const string &version)
{
    version_ = version;
}

void HttpRequestGenerator::setHost(const string &host)
{
    header_->establish("Host", host);
}

void HttpRequestGenerator::polishHeader()
{
    header_->insert("Connection", "keep-alive");
    if (contentLength_ > 0) {
        header_->remove("Transfer-Encoding");
        header_->establish("Content-Length", str(contentLength_));
    }
    else if (contentLength_ < 0) {
        header_->establish("Transfer-Encoding", "chunked");
    }
}

void HttpRequestGenerator::writeFirstLine(format &sink)
{
    sink << method_ << " " << path_ << " " << version_ << "\r\n";
}

}} // namespace cc::http
