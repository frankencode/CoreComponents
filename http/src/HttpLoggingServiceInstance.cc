/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/HttpLoggingServiceInstance>
#include <cc/HttpStatusLogStream>
#include <cc/HttpLoggingServiceRegistry>
#include <cc/NullStream>

namespace cc {

HttpLoggingServiceInstance::State::State(const MetaObject &config):
    verbosity_{readLoggingLevel(config("verbosity").to<String>())},
    errorStream_{NullStream{}},
    warningStream_{NullStream{}},
    noticeStream_{NullStream{}},
    infoStream_{NullStream{}},
    debugStream_{NullStream{}}
{
    if (verbosity_ >= LoggingLevel::Error  ) errorStream_   = HttpStatusLogStream{*this, LoggingLevel::Error};
    if (verbosity_ >= LoggingLevel::Warning) warningStream_ = HttpStatusLogStream{*this, LoggingLevel::Warning};
    if (verbosity_ >= LoggingLevel::Notice ) noticeStream_  = HttpStatusLogStream{*this, LoggingLevel::Notice};
    if (verbosity_ >= LoggingLevel::Info   ) infoStream_    = HttpStatusLogStream{*this, LoggingLevel::Info};
    if (verbosity_ >= LoggingLevel::Debug  ) debugStream_   = HttpStatusLogStream{*this, LoggingLevel::Debug};
}

HttpLoggingServiceInstance::HttpLoggingServiceInstance(const MetaObject &config)
{
     auto b = HttpLoggingServiceRegistry{}.serviceByName(config.className()).createInstance(config);
     *this = b;
}

} // namespace cc
