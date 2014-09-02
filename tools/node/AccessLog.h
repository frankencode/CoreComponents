/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNODE_ACCESSLOG_H
#define FLUXNODE_ACCESSLOG_H

#include "Log.h"

namespace flux { template<class> class ThreadLocalSingleton; }

namespace fluxnode
{

class AccessLog: public Log
{
private:
	friend class ThreadLocalSingleton<AccessLog>;
};

AccessLog *accessLog();

} // namespace fluxnode

#endif // FLUXNODE_ACCESSLOG_H
