/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Process>
#include <cc/File>
#include <cc/NullStream>
#include <cc/Mutex>
#include <cc/Guard>
#include <cc/stdio>
#include "SystemLog.h"
#include "Log.h"

namespace ccnode {

Log::Log():
    mutex_{Mutex::create()},
    config_{LogConfig::loadDefault()}
{}

Log::~Log()
{}

void Log::open(const LogConfig *config)
{
    config_ = config;
    open();
}

void Log::open()
{
    Guard<Mutex> guard(mutex_);

    if (path() != "") {
        errorStream_   =
        warningStream_ =
        noticeStream_  =
        infoStream_    =
        debugStream_   = File::open(path(), FileOpen::WriteOnly|FileOpen::Append|FileOpen::Create);
    }
    else if (Process::isDaemonized()) {
        errorStream_   = systemLog()->errorStream();
        warningStream_ = systemLog()->warningStream();
        noticeStream_  = systemLog()->noticeStream();
        infoStream_    = systemLog()->infoStream();
        debugStream_   = systemLog()->debugStream();
    }
    else {
        errorStream_   =
        warningStream_ =
        noticeStream_  =
        infoStream_    =
        debugStream_   = stdErr();
    }

    if (level() < LogLevel::Error)   errorStream_   = NullStream::instance();
    if (level() < LogLevel::Warning) warningStream_ = NullStream::instance();
    if (level() < LogLevel::Notice)  noticeStream_  = NullStream::instance();
    if (level() < LogLevel::Info)    infoStream_    = NullStream::instance();
    if (level() < LogLevel::Debug)   debugStream_   = NullStream::instance();
}

Stream *Log::errorStream()   const { Guard<Mutex> guard(mutex_); return errorStream_; }
Stream *Log::warningStream() const { Guard<Mutex> guard(mutex_); return warningStream_; }
Stream *Log::noticeStream()  const { Guard<Mutex> guard(mutex_); return noticeStream_; }
Stream *Log::infoStream()    const { Guard<Mutex> guard(mutex_); return infoStream_; }
Stream *Log::debugStream()   const { Guard<Mutex> guard(mutex_); return debugStream_; }

} // namespace ccnode
