/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXNODE_LOGMASTER_H
#define FLUXNODE_LOGMASTER_H

#include <flux/Thread.h>
#include <flux/Singleton.h>
#include <flux/Mutex.h>
#include <flux/Channel.h>
#include <flux/Timer.h>
#include <flux/Map.h>
#include <flux/Set.h>

namespace fluxnode
{

using namespace flux;

class Log;

class LogMaster: public Thread, public Singleton<LogMaster>
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

inline LogMaster *logMaster() { return LogMaster::instance(); }

} // namespace fluxnode

#endif // FLUXNODE_LOGMASTER_H
