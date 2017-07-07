/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/exceptions>
#include "Log.h"
#include "LogConfig.h"

namespace ccnode {

int decodeLogLevel(String levelName)
{
    if (levelName == "debug")   return DebugLogLevel;
    if (levelName == "info")    return InfoLogLevel;
    if (levelName == "notice")  return NoticeLogLevel;
    if (levelName == "warning") return WarningLogLevel;
    if (levelName == "error")   return ErrorLogLevel;
    if (levelName == "silent")  return SilentLogLevel;
    if (levelName == "")        return DefaultLogLevel;
    throw UsageError("Unknown log level \"" + levelName + "\"");
    return 0;
}

Ref<LogConfig> LogConfig::loadDefault()
{
    return new LogConfig();
}

Ref<LogConfig> LogConfig::load(MetaObject *config)
{
    return new LogConfig(config);
}

LogConfig::LogConfig():
    level_(DefaultLogLevel),
    retentionPeriod_(days(30)),
    rotationInterval_(days(1))
{}

LogConfig::LogConfig(MetaObject *config):
    path_(config->value("path")),
    level_(decodeLogLevel(config->value("level", ""))),
    retentionPeriod_(config->value("retention")),
    rotationInterval_(config->value("rotation"))
{}

} // namespace ccnode
