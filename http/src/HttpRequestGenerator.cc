/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Format>
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

void HttpRequestGenerator::setMethod(const String &method)
{
    method_ = method;
}

void HttpRequestGenerator::setPath(const String &path)
{
    path_ = path;
}

void HttpRequestGenerator::setVersion(const String &version)
{
    version_ = version;
}

void HttpRequestGenerator::setHost(const String &host)
{
    header_->establish("Host", host);
}

void HttpRequestGenerator::polishHeader()
{
    header_->insert("Connection", "keep-alive");
}

void HttpRequestGenerator::writeFirstLine(Format &sink)
{
    sink << method_ << " " << path_ << " " << version_ << "\r\n";
}

}} // namespace cc::http
