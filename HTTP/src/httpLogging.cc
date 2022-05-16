/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/httpLogging>
#include <cc/Casefree>
#include <cc/Date>
#include <cc/Format>

namespace cc {

LoggingLevel readLoggingLevel(const String &text)
{
    Casefree h{text};
    LoggingLevel level = LoggingLevel::Default;
    if (h == "silent") level = LoggingLevel::Silent;
    else if (h == "error") level = LoggingLevel::Error;
    else if (h == "warning") level = LoggingLevel::Warning;
    else if (h == "notice") level = LoggingLevel::Notice;
    else if (h == "info") level = LoggingLevel::Info;
    else if (h == "debug") level = LoggingLevel::Debug;
    return level;
}

String formatDeliveryNotice(const HttpClientConnection &client, const HttpRequest &request, HttpStatus statusCode, long long bytesWritten, const String &statusMessage)
{
    String requestHost = request ? request.host() : "";
    String requestLine = request ? request.line() : "";
    double requestTime = request ? request.time() : client.arrivalTime();
    String userAgent   = request ? request.header().value("User-Agent") : statusMessage;

    return Format{}
        << client.peerAddress().networkAddress() << " "
        << Date{requestTime}.toString() << " "
        << "\"" << requestHost << "\" "
        << "\"" << requestLine << "\" "
        << +statusCode << " " << bytesWritten << " "
        << "\"" << userAgent << "\" "
        << client.priority()
        << nl;
}

const char *syslogLoggingServiceName()
{
    return "SystemLog";
}

const char *foregroundLoggingServiceName()
{
    return "ForegroundLog";
}

} // namespace cc
