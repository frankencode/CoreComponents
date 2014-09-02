/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/exceptions.h>
#include "Log.h"
#include "LogConfig.h"

namespace fluxnode
{

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

LogConfig::LogConfig()
	: level_(DefaultLogLevel),
	  retentionPeriod_(days(30)),
	  rotationInterval_(days(1))
{}

LogConfig::LogConfig(MetaObject *config)
	: path_(config->value("path")),
	  level_(decodeLogLevel(config->value("level", ""))),
	  retentionPeriod_(config->value("retention")),
	  rotationInterval_(config->value("rotation"))
{}

} // namespace fluxnode
