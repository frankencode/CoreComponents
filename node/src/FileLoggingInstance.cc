/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/http/FileLoggingInstance>
#include <cc/File>
#include <cc/NullStream>

namespace cc {
namespace http {

Ref<FileLoggingInstance> FileLoggingInstance::create(const MetaObject *config)
{
    return new FileLoggingInstance{config};
}

FileLoggingInstance::FileLoggingInstance(const MetaObject *config):
    LoggingInstance{config}
{
    string path = config->value("path");
    if (path == "") sink_ = NullStream::instance();
    else sink_ = File::open(path, FileOpen::WriteOnly|FileOpen::Append|FileOpen::Create);
}

void FileLoggingInstance::logDelivery(const HttpServerConnection *client, int statusCode, size_t bytesWritten, const string &statusMessage) const
{
    sink_->write(
        formatDeliveryLogMessage(client, statusCode, bytesWritten, statusMessage)
    );
}

void FileLoggingInstance::logMessage(const string &message, LoggingLevel level) const
{
    if (verbosity() >= level) sink_->write(message);
}

}} // namespace cc::http
