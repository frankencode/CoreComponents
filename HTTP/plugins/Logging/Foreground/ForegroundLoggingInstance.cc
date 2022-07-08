/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/ForegroundLoggingInstance>
#include <cc/IoStream>

namespace cc {

struct ForegroundLoggingInstance::State: public HttpLoggingServiceInstance::State
{
    State(const MetaObject &config):
        HttpLoggingServiceInstance::State{config}
    {}

    void logDelivery(const HttpClientConnection &client, const HttpRequest &request, HttpStatus status, long long bytesWritten, const String &statusMessage) const override
    {
        IoStream::output().write(
            formatDeliveryNotice(client, request, status, bytesWritten, statusMessage)
        );
    }

    void logMessage(const String &message, LoggingLevel level) const override
    {
        if (verbosity() >= level) IoStream::error().write(message);
    }
};

ForegroundLoggingInstance::ForegroundLoggingInstance(const MetaObject &config):
    HttpLoggingServiceInstance{new State{config}}
{}

} // namespace cc
