/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FNODE_ACCESSLOG_H
#define FNODE_ACCESSLOG_H

#include <fkit/ThreadLocalSingleton.h>
#include <fkit/Format.h>
#include "Log.h"

namespace fnode
{

class AccessLog: public Log, public ThreadLocalSingleton<AccessLog>
{
private:
	friend class ThreadLocalSingleton<AccessLog>;
};

inline AccessLog *accessLog() { return AccessLog::instance(); }

inline Format accessError()   { return Format(accessLog()->errorStream()); }
inline Format accessWarning() { return Format(accessLog()->warningStream()); }
inline Format accessNotice()  { return Format(accessLog()->noticeStream()); }
inline Format accessInfo()    { return Format(accessLog()->infoStream()); }
inline Format accessDebug()   { return Format(accessLog()->debugStream()); }

inline Format accessError  (String format) { return Format(format, accessLog()->errorStream()); }
inline Format accessWarning(String format) { return Format(format, accessLog()->warningStream()); }
inline Format accessNotice (String format) { return Format(format, accessLog()->noticeStream()); }
inline Format accessInfo   (String format) { return Format(format, accessLog()->infoStream()); }
inline Format accessDebug  (String format) { return Format(format, accessLog()->debugStream()); }

} // namespace fnode

#endif // FNODE_ACCESSLOG_H
