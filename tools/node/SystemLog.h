/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNODE_SYSTEMLOG_H
#define FLUXNODE_SYSTEMLOG_H

#include <syslog.h>
#include <flux/Stream>

namespace flux { template<class> class ThreadLocalSingleton; }

namespace fluxnode
{

using namespace flux;

class SystemLog: public Object
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

SystemLog *systemLog();

} // namespace fluxnode

#endif // FLUXNODE_SYSTEMLOG_H
