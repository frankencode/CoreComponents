/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/Guard.h>
#include <flux/Singleton.h>
#include <flux/File.h>
#include <flux/System.h>
#include "Log.h"
#include "NodeConfig.h"
#include "LogMaster.h"

namespace fluxnode
{

LogMaster::LogMaster()
	: mutex_(Mutex::create()),
	  timerByPath_(TimerByPath::create()),
	  rotate_(Rotate::create())
{
	start();
}

LogMaster::~LogMaster()
{
	rotate_->push(Ref<Logs>());
	wait();
}

void LogMaster::registerLog(Log *log)
{
	if (log->path() == "") return;
	Guard<Mutex> guard(mutex_);
	Ref<RotateTimer> timer;
	if (!timerByPath_->lookup(log->path(), &timer)) {
		Ref<Logs> logs = Logs::create();
		logs->insert(log);
		timer = RotateTimer::start(log->rotationInterval(), rotate_, logs);
	}
	else {
		timer->tick()->insert(log);
	}
}

void LogMaster::unregisterLog(Log *log)
{
	if (log->path() == "") return;
	Guard<Mutex> guard(mutex_);
	Ref<RotateTimer> timer;
	if (timerByPath_->lookup(log->path(), &timer)) {
		Ref<Logs> logs = timer->tick();
		logs->remove(log);
		if (logs->count() == 0) timerByPath_->remove(log->path());
	}
}

void LogMaster::rotate(Log *log)
{
	double startTime = System::now();
	int n = 1;
	while (File::exists(log->path() + "." + str(n))) ++n;
	for (int i = n; i > 0; --i) {
		String path = log->path() + "." + str(i);
		if (startTime - FileStatus::read(path)->lastModified() > log->retentionPeriod())
			File::unlink(path);
		else
			File::rename(path, log->path() + "." + str(i + 1));
	}
	File::rename(log->path(), log->path() + ".1");
	File::create(log->path());
}

void LogMaster::run()
{
	Ref<Logs> logs;
	while (rotate_->pop(&logs)) {
		Guard<Mutex> guard(mutex_);
		if (logs->count() == 0) continue;
		rotate(logs->at(0));
		for (int i = 0; i < logs->count(); ++i)
			logs->at(i)->open();
	}
}

LogMaster *logMaster() { return Singleton<LogMaster>::instance(); }

} // namespace fluxnode
