/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/LogConfig>
#include <cc/node/Log>
#include <cc/exceptions>
#include <cc/Date>

namespace cc {
namespace node {

Ref<LogConfig> LogConfig::loadDefault()
{
    return new LogConfig;
}

Ref<LogConfig> LogConfig::load(MetaObject *config)
{
    return new LogConfig{config};
}

LogLevel LogConfig::decodeLogLevel(const String &levelName)
{
    if (levelName == "debug")   return LogLevel::Debug;
    if (levelName == "info")    return LogLevel::Info;
    if (levelName == "notice")  return LogLevel::Notice;
    if (levelName == "warning") return LogLevel::Warning;
    if (levelName == "error")   return LogLevel::Error;
    if (levelName == "silent")  return LogLevel::Silent;
    if (levelName == "")        return LogLevel::Default;
    throw UsageError{"Unknown log level \"" + levelName + "\""};
    return LogLevel::Default;
}

LogConfig::LogConfig():
    level_{LogLevel::Default},
    retentionPeriod_{days(30)},
    rotationInterval_{days(1)}
{}

LogConfig::LogConfig(MetaObject *config):
    path_{config->value("path")},
    level_{decodeLogLevel(config->value("level", ""))},
    retentionPeriod_{config->value("retention")},
    rotationInterval_{config->value("rotation")}
{}

}} // namespace cc::node
