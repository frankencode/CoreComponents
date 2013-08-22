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

inline Format error  (String format) { return Format(format, errorLog()->errorStream()); }
inline Format warning(String format) { return Format(format, errorLog()->warningStream()); }
inline Format notice (String format) { return Format(format, errorLog()->noticeStream()); }
inline Format info   (String format) { return Format(format, errorLog()->infoStream()); }
inline Format debug  (String format) { return Format(format, errorLog()->debugStream()); }

} // namespace fnode

#endif // FNODE_ERRORLOG_H
