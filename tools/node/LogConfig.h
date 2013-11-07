/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXNODE_LOGCONFIG_H
#define FLUXNODE_LOGCONFIG_H

#include <flux/Yason.h>

namespace fluxnode
{

using namespace flux;

class LogConfig: public Object
{
public:
	static Ref<LogConfig> loadDefault();
	static Ref<LogConfig> load(YasonObject *config);

	inline String path() const { return path_; }
	inline int level() const { return level_; }
	inline double retentionPeriod() const { return retentionPeriod_; }
	inline double rotationInterval() const { return rotationInterval_; }

private:
	LogConfig();
	LogConfig(YasonObject *config);

	String path_;
	int level_;
	double retentionPeriod_;
	double rotationInterval_;
};

} // namespace fluxnode

#endif // FLUXNODE_LOGCONFIG_H
