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
public:
    static ErrorLog *instance();

private:
    friend class ThreadLocalSingleton<ErrorLog>;
};

inline Format error()   { return Format(ErrorLog::instance()->errorStream()); }
inline Format warning() { return Format(ErrorLog::instance()->warningStream()); }
inline Format notice()  { return Format(ErrorLog::instance()->noticeStream()); }
inline Format info()    { return Format(ErrorLog::instance()->infoStream()); }
inline Format debug()   { return Format(ErrorLog::instance()->debugStream()); }

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
