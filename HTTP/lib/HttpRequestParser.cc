/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/HttpRequestParser>
#include <cc/System>
#include <cc/LineSource>
#include <cc/Casefree>

namespace cc {

struct HttpRequestParser::State: public HttpMessageParser::State
{
    State(const Stream &stream):
        HttpMessageParser::State{stream}
    {}

    void onFirstLineReceived(const String &line, HttpMessage &message) override
    {
        HttpRequest request = message.as<HttpRequest>();
        request->line_ = line;

        if (line.count(' ') != 2) throw HttpBadRequest{};

        long i0 = 0, i1 = 0;
        line.find(' ', &i1);
        request->method_ = line.copy(i0, i1);
        i0 = ++i1;
        line.find(' ', &i1);
        request->uri_ = line.copy(i0, i1);
        request->version_ = line.copy(i1 + 1, line.count());
        request->majorVersion_ = 1;
        request->minorVersion_ = 0;

        List<String> parts = request->version_.split('/');
        if (parts.count() >= 2) {
            if (Casefree{parts.at(0)} != "HTTP") throw HttpUnsupportedVersion{};
            parts = parts.at(1).split('.');
            if (parts.count() >= 2) {
                request->majorVersion_ = parts.at(0).toInt();
                request->minorVersion_ = parts.at(1).toInt();
            }
        }

        if (request->majorVersion_ > 1) throw HttpUnsupportedVersion{};
    }

    void onHeaderReceived(HttpMessage &message) override
    {
        HttpRequest request = message.as<HttpRequest>();
        request->host_ = request->header_.value("Host").downcased();
        if (request->host_ == "") throw HttpBadRequest{};
    }
};

HttpRequestParser::HttpRequestParser(const Stream &stream):
    HttpMessageParser{new State{stream}}
{}

HttpRequest HttpRequestParser::readRequest()
{
    HttpRequest request{New{}};
    request->time_ = System::now();
    readMessage(&request);
    return request;
}

} // namespace cc
