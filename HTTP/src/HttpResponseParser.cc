/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/HttpResponseParser>
#include <cc/LineSource>
#include <cc/DEBUG>

namespace cc {

struct HttpResponseParser::State: public HttpMessageParser::State
{
    State(const Stream &stream):
        HttpMessageParser::State{stream}
    {}

    void onFirstLineReceived(const String &line, HttpMessage &message) override
    {
        HttpResponse response = message.as<HttpResponse>();
        List<String> parts = line.split(' ');
        if (parts.count() > 0) {
            response->version_ = parts.first();
            parts.popFront();
            response->version_.upcase();
        }

        if (
            response->version_ != "HTTP/1.1" &&
            response->version_ != "HTTP/1.0"
        ) {
            throw HttpUnsupportedVersion{};
        }

        if (parts.count() > 0) {
            response->status_ = static_cast<HttpStatus>(parts.first().toInt());
            parts.popFront();
        }

        if (parts.count() > 0) {
            response->reasonPhrase_ = parts.join(' ');
        }
    }
};

HttpResponseParser::HttpResponseParser(const Stream &stream):
    HttpMessageParser{new State{stream}}
{}

HttpResponse HttpResponseParser::readResponse()
{
    HttpResponse response{New{}};
    me().readMessage(&response);
    return response;
}

} // namespace cc
