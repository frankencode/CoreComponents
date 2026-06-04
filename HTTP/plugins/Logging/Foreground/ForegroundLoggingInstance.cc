/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
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
        stdOutput().write(
            formatDeliveryNotice(client, request, status, bytesWritten, statusMessage)
        );
    }

    void logMessage(const String &message, LoggingLevel level) const override
    {
        if (verbosity() >= level) stdError().write(message);
    }
};

ForegroundLoggingInstance::ForegroundLoggingInstance(const MetaObject &config):
    HttpLoggingServiceInstance{new State{config}}
{}

} // namespace cc
