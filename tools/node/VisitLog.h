/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNODE_VISITLOG_H
#define FLUXNODE_VISITLOG_H

#include <flux/ThreadLocalSingleton>
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
