/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXNODE_SYSTEMLOG_H
#define FLUXNODE_SYSTEMLOG_H

#include <syslog.h>
#include <flux/Stream.h>
#include <flux/ThreadLocalSingleton.h>

namespace fluxnode
{

using namespace flux;

class SystemLog: public Object, public ThreadLocalSingleton<SystemLog>
{
public:
	static void open(String identifier, int option, int facility);

	inline Stream *emergencyStream() const { return emergencyStream_; }
	inline Stream *alertStream()     const { return alertStream_; }
	inline Stream *criticalStream()  const { return criticalStream_; }
	inline Stream *errorStream()     const { return errorStream_; }
	inline Stream *warningStream()   const { return warningStream_; }
	inline Stream *noticeStream()    const { return noticeStream_; }
	inline Stream *infoStream()      const { return infoStream_; }
	inline Stream *debugStream()     const { return debugStream_; }

private:
	friend class ThreadLocalSingleton<SystemLog>;
	SystemLog();

	Ref<Stream> emergencyStream_;
	Ref<Stream> alertStream_;
	Ref<Stream> criticalStream_;
	Ref<Stream> errorStream_;
	Ref<Stream> warningStream_;
	Ref<Stream> noticeStream_;
	Ref<Stream> infoStream_;
	Ref<Stream> debugStream_;
};

inline SystemLog *systemLog() { return SystemLog::instance(); }

} // namespace fluxnode

#endif // FLUXNODE_SYSTEMLOG_H
