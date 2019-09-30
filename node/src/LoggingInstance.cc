/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/LoggingInstance>
#include <cc/node/StatusLogStream>
#include <cc/NullStream>

namespace cc {
namespace node {

LoggingInstance::LoggingInstance(const MetaObject *config):
    verbosity_{readLoggingLevel(config->value("verbosity"))},
    errorStream_{NullStream::instance()},
    warningStream_{NullStream::instance()},
    noticeStream_{NullStream::instance()},
    infoStream_{NullStream::instance()},
    debugStream_{NullStream::instance()}
{
    if (verbosity_ >= LoggingLevel::Error  ) errorStream_   = StatusLogStream::open(this, LoggingLevel::Error);
    if (verbosity_ >= LoggingLevel::Warning) warningStream_ = StatusLogStream::open(this, LoggingLevel::Warning);
    if (verbosity_ >= LoggingLevel::Notice ) noticeStream_  = StatusLogStream::open(this, LoggingLevel::Notice);
    if (verbosity_ >= LoggingLevel::Info   ) infoStream_    = StatusLogStream::open(this, LoggingLevel::Info);
    if (verbosity_ >= LoggingLevel::Debug  ) debugStream_   = StatusLogStream::open(this, LoggingLevel::Debug);
}

LoggingLevel LoggingInstance::verbosity() const
{
    return verbosity_;
}

}} // namespace cc::node
