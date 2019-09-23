/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/logging>
#include <cc/node/HttpServerConnection>
#include <cc/System>
#include <cc/Date>
#include <cc/Format>

namespace cc {
namespace node {

LoggingType readLoggingType(const String &s)
{
    if (s->equalsCaseInsensitive("delivery")) return LoggingType::Delivery;
    else if (s->equalsCaseInsensitive("status")) return LoggingType::Status;
    return LoggingType::Combined;
}

LoggingLevel readLoggingLevel(const String &s)
{
    if (s->equalsCaseInsensitive("silent")) return LoggingLevel::Silent;
    else if (s->equalsCaseInsensitive("error")) return LoggingLevel::Error;
    else if (s->equalsCaseInsensitive("warning")) return LoggingLevel::Warning;
    else if (s->equalsCaseInsensitive("notice")) return LoggingLevel::Notice;
    else if (s->equalsCaseInsensitive("info")) return LoggingLevel::Info;
    else if (s->equalsCaseInsensitive("debug")) return LoggingLevel::Debug;
    return LoggingLevel::Default;
}

String formatDeliveryLogMessage(const HttpServerConnection *client, int statusCode, size_t bytesWritten, const String &statusMessage)
{
    const HttpRequest *request = client->request();
    String requestHost = request ? request->host() : "";
    String requestLine = request ? request->line() : "";
    double requestTime = request ? request->time() : System::now();
    String userAgent   = request ? request->value("User-Agent") : statusMessage;

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

}} // namespace cc::node
