/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/UserError.h>
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
	throw UserError("Unknown log level \"" + levelName + "\"");
	return 0;
}

Ref<LogConfig> LogConfig::loadDefault()
{
	return new LogConfig();
}

Ref<LogConfig> LogConfig::load(YasonObject *config)
{
	return new LogConfig(config);
}

LogConfig::LogConfig()
	: level_(DefaultLogLevel),
	  retentionPeriod_(days(30)),
	  rotationInterval_(days(1))
{}

LogConfig::LogConfig(YasonObject *config)
	: path_(config->value("path")),
	  level_(decodeLogLevel(config->value("level", ""))),
	  retentionPeriod_(config->value("retention")),
	  rotationInterval_(config->value("rotation"))
{}

} // namespace fluxnode
