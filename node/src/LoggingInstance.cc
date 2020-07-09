/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/http/LoggingInstance>
#include <cc/http/LoggingRegistry>
#include <cc/http/StatusLogStream>
#include <cc/NullStream>

namespace cc {
namespace http {

Ref<LoggingInstance> LoggingInstance::load(const MetaObject *config)
{
    return LoggingRegistry::instance()->serviceByName(config->className())->createInstance(config);
}

LoggingInstance::LoggingInstance(const MetaObject *config):
    verbosity_{readLoggingLevel(config->value("verbosity"))},
    errorStream_{NullStream{}},
    warningStream_{NullStream{}},
    noticeStream_{NullStream{}},
    infoStream_{NullStream{}},
    debugStream_{NullStream{}}
{
    if (verbosity_ >= LoggingLevel::Error  ) errorStream_   = StatusLogStream{this, LoggingLevel::Error};
    if (verbosity_ >= LoggingLevel::Warning) warningStream_ = StatusLogStream{this, LoggingLevel::Warning};
    if (verbosity_ >= LoggingLevel::Notice ) noticeStream_  = StatusLogStream{this, LoggingLevel::Notice};
    if (verbosity_ >= LoggingLevel::Info   ) infoStream_    = StatusLogStream{this, LoggingLevel::Info};
    if (verbosity_ >= LoggingLevel::Debug  ) debugStream_   = StatusLogStream{this, LoggingLevel::Debug};
}

LoggingLevel LoggingInstance::verbosity() const
{
    return verbosity_;
}

}} // namespace cc::http
