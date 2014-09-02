/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/ThreadLocalSingleton.h>
#include <flux/stream/LineBuffer.h>
#include "SystemLogStream.h"
#include "SystemLog.h"

namespace fluxnode
{

using namespace flux::stream;

SystemLog::SystemLog()
	: emergencyStream_(LineBuffer::open(SystemLogStream::open(LOG_EMERG))),
	  alertStream_    (LineBuffer::open(SystemLogStream::open(LOG_ALERT))),
	  criticalStream_ (LineBuffer::open(SystemLogStream::open(LOG_CRIT))),
	  errorStream_    (LineBuffer::open(SystemLogStream::open(LOG_ERR))),
	  warningStream_  (LineBuffer::open(SystemLogStream::open(LOG_WARNING))),
	  noticeStream_   (LineBuffer::open(SystemLogStream::open(LOG_NOTICE))),
	  infoStream_     (LineBuffer::open(SystemLogStream::open(LOG_INFO))),
	  debugStream_    (LineBuffer::open(SystemLogStream::open(LOG_DEBUG)))
{}

void SystemLog::open(String identifier, int option, int facility)
{
	openlog(identifier, option, facility);
}

SystemLog *systemLog() { return ThreadLocalSingleton<SystemLog>::instance(); }

} // namespace fluxnode
