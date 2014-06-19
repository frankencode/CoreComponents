/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXNODE_ERRORLOG_H
#define FLUXNODE_ERRORLOG_H

#include <flux/Format.h>
#include <flux/str.h>
#include "Log.h"

namespace flux { template<class> class ThreadLocalSingleton; }

namespace fluxnode
{

class ErrorLog: public Log
{
private:
	friend class ThreadLocalSingleton<ErrorLog>;
};

ErrorLog *errorLog();

inline Format error()   { return Format(errorLog()->errorStream()); }
inline Format warning() { return Format(errorLog()->warningStream()); }
inline Format notice()  { return Format(errorLog()->noticeStream()); }
inline Format info()    { return Format(errorLog()->infoStream()); }
inline Format debug()   { return Format(errorLog()->debugStream()); }

#define FLUXNODE_LOG(sink) \
	sink() << "(" << inum(thread()->id(), 62) << ") " << String(__FILE__)->baseName() << ": "

#define FLUXNODE_ERROR()   FLUXNODE_LOG(error)
#define FLUXNODE_WARNING() FLUXNODE_LOG(warning)
#define FLUXNODE_NOTICE()  FLUXNODE_LOG(notice)
#define FLUXNODE_INFO()    FLUXNODE_LOG(info)
#ifndef NDEBUG
#define FLUXNODE_DEBUG()   FLUXNODE_LOG(debug)
#else
#define FLUXNODE_DEBUG()   NullFormat()
#endif

} // namespace fluxnode

#endif // FLUXNODE_ERRORLOG_H
