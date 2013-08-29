/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FNODE_ERRORLOG_H
#define FNODE_ERRORLOG_H

#include <fkit/ThreadLocalSingleton.h>
#include <fkit/Format.h>
#include "Log.h"

namespace fnode
{

class ErrorLog: public Log, public ThreadLocalSingleton<ErrorLog>
{
private:
	friend class ThreadLocalSingleton<ErrorLog>;
};

inline ErrorLog *errorLog() { return ErrorLog::instance(); }

inline Format error()   { return Format(errorLog()->errorStream()); }
inline Format warning() { return Format(errorLog()->warningStream()); }
inline Format notice()  { return Format(errorLog()->noticeStream()); }
inline Format info()    { return Format(errorLog()->infoStream()); }
inline Format debug()   { return Format(errorLog()->debugStream()); }

#define FNODE_LOG(sink) \
  sink() << (thread()->name() == "") ? str(thread()->id()) : pthread()->name() << ":" \
         << __FILE__ << ":" << __LINE__ << ": "

#define FNODE_ERROR()   FNODE_LOG(error)
#define FNODE_WARNING() FNODE_LOG(warning)
#define FNODE_NOTICE()  FNODE_LOG(notice)
#define FNODE_INFO()    FNODE_LOG(info)
#define FNODE_DEBUG()   FNODE_LOG(debug)

} // namespace fnode

#endif // FNODE_ERRORLOG_H
