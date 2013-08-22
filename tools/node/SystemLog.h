/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FNODE_SYSTEMLOG_H
#define FNODE_SYSTEMLOG_H

#include <fkit/Stream.h>
#include <fkit/Singleton.h>
#include <syslog.h>

namespace fnode
{

using namespace fkit;

class SystemLog: public Object, public Singleton<SystemLog>
{
public:
	void open(const char *identifier, int option, int facility);

	inline Stream *emergencyStream() const { return emergencyStream_; }
	inline Stream *alertStream()     const { return alertStream_; }
	inline Stream *criticalStream()  const { return criticalStream_; }
	inline Stream *errorStream()     const { return errorStream_; }
	inline Stream *warningStream()   const { return warningStream_; }
	inline Stream *noticeStream()    const { return noticeStream_; }
	inline Stream *infoStream()      const { return infoStream_; }
	inline Stream *debugStream()     const { return debugStream_; }

private:
	friend class Singleton<SystemLog>;
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

} // namespace fnode

#endif // FNODE_SYSTEMLOG_H
