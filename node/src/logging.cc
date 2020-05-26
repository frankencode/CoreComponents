/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/http/logging>
#include <cc/http/HttpServerConnection>
#include <cc/System>
#include <cc/Date>
#include <cc/Format>

namespace cc {
namespace http {

LoggingLevel readLoggingLevel(const string &s)
{
    if (s->equalsCaseInsensitive("silent")) return LoggingLevel::Silent;
    else if (s->equalsCaseInsensitive("error")) return LoggingLevel::Error;
    else if (s->equalsCaseInsensitive("warning")) return LoggingLevel::Warning;
    else if (s->equalsCaseInsensitive("notice")) return LoggingLevel::Notice;
    else if (s->equalsCaseInsensitive("info")) return LoggingLevel::Info;
    else if (s->equalsCaseInsensitive("debug")) return LoggingLevel::Debug;
    return LoggingLevel::Default;
}

string formatDeliveryLogMessage(const HttpServerConnection *client, int statusCode, size_t bytesWritten, const string &statusMessage)
{
    const HttpRequest *request = client->request();
    string requestHost = request ? request->host() : "";
    string requestLine = request ? request->line() : "";
    double requestTime = request ? request->time() : System::now();
    string userAgent   = request ? request->value("User-Agent") : statusMessage;

    return Format{}
        << client->address()->networkAddress() << " "
        << Date::breakdown(requestTime)->toString() << " "
        << "\"" << requestHost << "\" "
        << "\"" << requestLine << "\" "
        << statusCode << " " << bytesWritten << " "
        << "\"" << userAgent << "\" "
        << client->priority()
        << nl;
}

}} // namespace cc::http
