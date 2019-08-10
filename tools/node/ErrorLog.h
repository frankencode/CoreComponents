/*
 * Copyright (C) 2007-2019 Frank Mertens.
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

#define CCNODE_LOG(sink) \
    sink << "(" << inum(uint64_t(Thread::self()->id()), 62) << ") " << String{__FILE__}->baseName() << ": "

#define CCNODE_ERROR()   CCNODE_LOG(Format{ErrorLog::instance()->errorStream()  })
#define CCNODE_WARNING() CCNODE_LOG(Format{ErrorLog::instance()->warningStream()})
#define CCNODE_NOTICE()  CCNODE_LOG(Format{ErrorLog::instance()->noticeStream() })
#define CCNODE_INFO()    CCNODE_LOG(Format{ErrorLog::instance()->infoStream()   })
#ifndef NDEBUG
#define CCNODE_DEBUG()   CCNODE_LOG(Format{ErrorLog::instance()->debugStream()  })
#else
#define CCNODE_DEBUG()   NullFormat()
#endif

} // namespace ccnode
