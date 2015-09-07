/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/Process>
#include <flux/NullStream>
#include <flux/Mutex>
#include <flux/Guard>
#include <flux/stdio>
#include "LogMaster.h"
#include "SystemLog.h"
#include "Log.h"

namespace fluxnode {

Log::Log():
    logMaster_(logMaster()),
    mutex_(Mutex::create()),
    config_(LogConfig::loadDefault())
{}

Log::~Log()
{
    logMaster_->unregisterLog(this);
}

void Log::open(LogConfig *config)
{
    config_ = config;
    logMaster_->registerLog(this);
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
        debugStream_   = File::open(path(), File::WriteOnly|File::Append);
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

    if (level() < ErrorLogLevel)   errorStream_   = nullStream();
    if (level() < WarningLogLevel) warningStream_ = nullStream();
    if (level() < NoticeLogLevel)  noticeStream_  = nullStream();
    if (level() < InfoLogLevel)    infoStream_    = nullStream();
    if (level() < DebugLogLevel)   debugStream_   = nullStream();
}

Stream *Log::errorStream()   const { Guard<Mutex> guard(mutex_); return errorStream_; }
Stream *Log::warningStream() const { Guard<Mutex> guard(mutex_); return warningStream_; }
Stream *Log::noticeStream()  const { Guard<Mutex> guard(mutex_); return noticeStream_; }
Stream *Log::infoStream()    const { Guard<Mutex> guard(mutex_); return infoStream_; }
Stream *Log::debugStream()   const { Guard<Mutex> guard(mutex_); return debugStream_; }

} // namespace fluxnode
