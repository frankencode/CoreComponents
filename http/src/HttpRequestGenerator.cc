/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/HttpRequestGenerator>

namespace cc {

struct HttpRequestGenerator::State: public HttpMessageGenerator::State
{
    State(const Stream &stream):
        HttpMessageGenerator::State{stream}
    {}

    void polishHeader() override
    {
        header_.insert("Connection", "keep-alive");
        if (contentLength_ > 0) {
            header_.remove("Transfer-Encoding");
            header_.establish("Content-Length", str(contentLength_));
        }
        else if (contentLength_ < 0) {
            header_.establish("Transfer-Encoding", "chunked");
        }
    }

    void writeFirstLine(Format &sink) override
    {
        if (method_ == "") sink << "GET ";
        else sink << method_ << " ";
        if (path_ == "") sink << "/ ";
        else sink << path_ << " ";
        if (version_ == "") sink << "HTTP/1.1\r\n";
        else sink << version_ << "\r\n";
    }

    String method_;
    String path_;
    String version_;
};

HttpRequestGenerator::HttpRequestGenerator(const Stream &stream):
    HttpMessageGenerator{new State{stream}}
{}

void HttpRequestGenerator::setMethod(const String &method)
{
    me().method_ = method;
}

void HttpRequestGenerator::setPath(const String &path)
{
    me().path_ = path;
}

void HttpRequestGenerator::setVersion(const String &version)
{
    me().version_ = version;
}

void HttpRequestGenerator::setHost(const String &host)
{
    me().setHeader("Host", host);
}

HttpRequestGenerator::State &HttpRequestGenerator::me()
{
    return Object::me.as<State>();
}

} // namespace cc
