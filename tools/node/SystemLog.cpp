/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/LineBuffer.h>
#include "SystemLogStream.h"
#include "SystemLog.h"

namespace fnode
{

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

} // namespace fnode
