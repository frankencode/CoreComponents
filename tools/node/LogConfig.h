/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNODE_LOGCONFIG_H
#define FLUXNODE_LOGCONFIG_H

#include <flux/MetaObject.h>

namespace fluxnode
{

using namespace flux;

class LogConfig: public Object
{
public:
	static Ref<LogConfig> loadDefault();
	static Ref<LogConfig> load(MetaObject *config);

	inline String path() const { return path_; }
	inline int level() const { return level_; }
	inline double retentionPeriod() const { return retentionPeriod_; }
	inline double rotationInterval() const { return rotationInterval_; }

private:
	LogConfig();
	LogConfig(MetaObject *config);

	String path_;
	int level_;
	double retentionPeriod_;
	double rotationInterval_;
};

} // namespace fluxnode

#endif // FLUXNODE_LOGCONFIG_H
