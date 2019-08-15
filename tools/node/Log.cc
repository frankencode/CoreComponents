/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/File>
#include <cc/NullStream>
#include "SystemLog.h"
#include "Log.h"

namespace ccnode {

Log::Log():
    config_{LogConfig::loadDefault()}
{}

Log::~Log()
{}

void Log::open(const LogConfig *config)
{
    config_ = config;

    if (path() != "") {
        errorStream_   =
        warningStream_ =
        noticeStream_  =
        infoStream_    =
        debugStream_   = File::open(path(), FileOpen::WriteOnly|FileOpen::Append|FileOpen::Create);
    }
    else if (stdErr()->isatty()) {
        errorStream_   =
        warningStream_ =
        noticeStream_  =
        infoStream_    =
        debugStream_   = stdErr();
    }
    else {
        errorStream_   = SystemLog::instance()->errorStream();
        warningStream_ = SystemLog::instance()->warningStream();
        noticeStream_  = SystemLog::instance()->noticeStream();
        infoStream_    = SystemLog::instance()->infoStream();
        debugStream_   = SystemLog::instance()->debugStream();
    }

    if (level() < LogLevel::Error)   errorStream_   = NullStream::instance();
    if (level() < LogLevel::Warning) warningStream_ = NullStream::instance();
    if (level() < LogLevel::Notice)  noticeStream_  = NullStream::instance();
    if (level() < LogLevel::Info)    infoStream_    = NullStream::instance();
    if (level() < LogLevel::Debug)   debugStream_   = NullStream::instance();
}

} // namespace ccnode
