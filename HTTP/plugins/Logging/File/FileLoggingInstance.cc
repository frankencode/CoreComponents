/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/FileLoggingInstance>
#include <cc/File>
#include <cc/NullStream>

namespace cc {

struct FileLoggingInstance::State: public HttpLoggingServiceInstance::State
{
    State(const MetaObject &config):
        HttpLoggingServiceInstance::State{config}
    {
        String path = config("path");
        if (path == "") sink_ = NullStream{};
        else sink_ = File{path, FileOpen::WriteOnly|FileOpen::Append|FileOpen::Create};
    }

    void logDelivery(const HttpClientConnection &client, const HttpRequest &request, HttpStatus status, long long bytesWritten, const String &statusMessage) const override
    {
        sink_.write(
            formatDeliveryNotice(client, request, status, bytesWritten, statusMessage)
        );
    }

    void logMessage(const String &message, LoggingLevel level) const override
    {
        if (verbosity() >= level) sink_.write(message);
    }

    mutable Stream sink_;
};

FileLoggingInstance::FileLoggingInstance(const MetaObject &config):
    HttpLoggingServiceInstance{new State{config}}
{}

} // namespace cc
