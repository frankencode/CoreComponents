/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXNODE_ACCESSLOG_H
#define FLUXNODE_ACCESSLOG_H

#include <flux/ThreadLocalSingleton.h>
#include <flux/Format.h>
#include "Log.h"

namespace fnode
{

class AccessLog: public Log, public ThreadLocalSingleton<AccessLog>
{
private:
	friend class ThreadLocalSingleton<AccessLog>;
};

inline AccessLog *accessLog() { return AccessLog::instance(); }

} // namespace fnode

#endif // FLUXNODE_ACCESSLOG_H
