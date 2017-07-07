/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Format>
#include <cc/Thread>
#include <cc/str>
#include "Log.h"

namespace cc { template<class> class ThreadLocalSingleton; }

namespace ccnode {

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

#define CCNODE_LOG(sink) \
    sink() << "(" << inum(uint64_t(Thread::self()->id()), 62) << ") " << String(__FILE__)->baseName() << ": "

#define CCNODE_ERROR()   CCNODE_LOG(error)
#define CCNODE_WARNING() CCNODE_LOG(warning)
#define CCNODE_NOTICE()  CCNODE_LOG(notice)
#define CCNODE_INFO()    CCNODE_LOG(info)
#ifndef NDEBUG
#define CCNODE_DEBUG()   CCNODE_LOG(debug)
#else
#define CCNODE_DEBUG()   NullFormat()
#endif

} // namespace ccnode
