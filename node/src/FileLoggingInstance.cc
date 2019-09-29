/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/FileLoggingInstance>
#include <cc/File>
#include <cc/NullStream>

namespace cc {
namespace node {

Ref<FileLoggingInstance> FileLoggingInstance::create(const MetaObject *config)
{
    return new FileLoggingInstance{config};
}

FileLoggingInstance::FileLoggingInstance(const MetaObject *config):
    LoggingInstance{config}
{
    String path = config->value("path");
    if (path == "") sink_ = NullStream::instance();
    else sink_ = File::open(path, FileOpen::WriteOnly|FileOpen::Append|FileOpen::Create);
}

void FileLoggingInstance::logDelivery(const HttpServerConnection *client, int statusCode, size_t bytesWritten, const String &statusMessage) const
{
    sink_->write(
        formatDeliveryLogMessage(client, statusCode, bytesWritten, statusMessage)
    );
}

void FileLoggingInstance::logStatus(const String &message, LoggingLevel level) const
{
    if (verbosity() >= level) sink_->write(message);
}

}} // namespace cc::node
