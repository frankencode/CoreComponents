/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/LoggingInstance>

namespace cc {
namespace node {

LoggingInstance::LoggingInstance(const MetaObject *config):
    loggingType_{readLoggingType(config->value("type"))},
    verbosity_{readLoggingLevel(config->value("level"))}
{}

LoggingType LoggingInstance::loggingType() const
{
    return loggingType_;
}

LoggingLevel LoggingInstance::verbosity() const
{
    return verbosity_;
}

}} // namespace cc::node
