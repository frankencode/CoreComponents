/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <fkit/Process.h>
#include <fkit/NullStream.h>
#include <fkit/Mutex.h>
#include <fkit/Guard.h>
#include <fkit/stdio.h>
#include "LogMaster.h"
#include "SystemLog.h"
#include "Log.h"

namespace fnode
{

Log::Log()
	: logMaster_(logMaster()),
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
		debugStream_   = err();
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

} // namespace fnode
