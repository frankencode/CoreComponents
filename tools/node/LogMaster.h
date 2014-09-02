/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNODE_LOGMASTER_H
#define FLUXNODE_LOGMASTER_H

#include <flux/Thread.h>
#include <flux/Mutex.h>
#include <flux/Channel.h>
#include <flux/Timer.h>
#include <flux/Map.h>
#include <flux/Set.h>

namespace flux { template<class> class Singleton; }

namespace fluxnode
{

using namespace flux;

class Log;

class LogMaster: public Thread
{
public:
	void registerLog(Log *log);
	void unregisterLog(Log *log);

private:
	friend class Singleton<LogMaster>;

	LogMaster();
	~LogMaster();
	void rotate(Log *log);
	virtual void run();

	Ref<Mutex> mutex_;

	typedef Set<Log *> Logs;
	typedef Timer< Ref<Logs> > RotateTimer;
	typedef Map<String, Ref<RotateTimer> > TimerByPath;
	Ref<TimerByPath> timerByPath_;

	typedef Channel< Ref<Logs> > Rotate;
	Ref<Rotate> rotate_;
};

LogMaster *logMaster();

} // namespace fluxnode

#endif // FLUXNODE_LOGMASTER_H
