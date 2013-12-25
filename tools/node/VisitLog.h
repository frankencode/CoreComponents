/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXNODE_VISITLOG_H
#define FLUXNODE_VISITLOG_H

#include <flux/ThreadLocalSingleton.h>
#include "Log.h"

namespace fluxnode
{

class VisitLog: public Log, public ThreadLocalSingleton<VisitLog>
{
private:
	friend class ThreadLocalSingleton<VisitLog>;
};

inline VisitLog *visitLog() { return VisitLog::instance(); }

} // namespace fluxnode

#endif // FLUXNODE_VISITLOG_H
