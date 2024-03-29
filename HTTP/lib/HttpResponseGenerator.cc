/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/HttpResponseGenerator>
#include <cc/httpDate>
#include <cc/System>

namespace cc {

struct HttpResponseGenerator::State: public HttpMessageGenerator::State
{
    State(const Stream &stream):
        HttpMessageGenerator::State{stream}
    {}

    void polishHeader() override
    {
        if (nodeVersion_ != "") header_.insert("Server", nodeVersion_);

        String now = httpDateToString(Date{System::now()});
        header_.insert("Date", now);

        if (status_ != HttpStatus::NotModified) {
            if (contentLength_ >= 0) {
                header_.remove("Transfer-Encoding");
                header_.establish("Content-Length", str(contentLength_));
            }
            else {
                header_.establish("Transfer-Encoding", "chunked");
            }
            header_.insert("Last-Modified", now);
        }
    }

    void writeFirstLine(Format &sink) override
    {
        sink << "HTTP/1.1 " << +status_ << " " << reasonPhrase_ << "\r\n";
    }

    long long bytesWritten() const
    {
        return (payload_) ? payload_.totalWritten() : bytesWritten_;
    }

    bool headerWritten() const
    {
        return headerWritten_;
    }

    HttpStatus status_ { 200 };
    String reasonPhrase_;
    String nodeVersion_;
};

HttpResponseGenerator::HttpResponseGenerator(const Stream &stream):
    HttpMessageGenerator{new State{stream}}
{}

void HttpResponseGenerator::setStatus(HttpStatus status, const String &reasonPhrase)
{
    me().status_ = status;
    me().reasonPhrase_ = (reasonPhrase == "") ? String{httpReasonPhrase(status)} : reasonPhrase;
}

void HttpResponseGenerator::setNodeVersion(const String &nodeVersion)
{
    me().nodeVersion_ = nodeVersion;
}

long long HttpResponseGenerator::bytesWritten() const
{
    return me().bytesWritten();
}

bool HttpResponseGenerator::delivered() const
{
    return me().headerWritten();
}

HttpStatus HttpResponseGenerator::status() const
{
    return me().status_;
}

HttpResponseGenerator::State &HttpResponseGenerator::me()
{
    return Object::me.as<State>();
}

const HttpResponseGenerator::State &HttpResponseGenerator::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
