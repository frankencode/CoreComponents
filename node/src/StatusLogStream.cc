/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/http/StatusLogStream>
#include <cc/http/LoggingInstance>

namespace cc {
namespace http {

StatusLogStream::Instance::Instance(LoggingInstance *sink, LoggingLevel level):
    sink_{sink},
    level_{level}
{}

void StatusLogStream::Instance::write(const CharArray *data)
{
    if (data->endsWith('\n'))
        sink_->logMessage(data, level_);
}

void StatusLogStream::Instance::write(const StringList &parts)
{
    write(parts->join());
}

}} // namespace cc::http
