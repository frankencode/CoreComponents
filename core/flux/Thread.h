/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_THREAD_H
#define FLUX_THREAD_H

#include <pthread.h>
#include <signal.h>
#include "SignalSet.h"
#include "ThreadLocalRef.h"
#include "String.h"

namespace flux
{

class ThreadFactory;
class Interrupt;

class Thread: public Object
{
public:
	static Thread *self();

	void start(int stackSize = -1);
	void wait();
	void kill(int signal);
	bool stillAlive() const;

	static void sleep(double duration);
	static void sleepUntil(double timeout);

	static void blockSignals(SignalSet *set);
	static void unblockSignals(SignalSet *set);

	pthread_t id() const { return tid_; }

protected:
	Thread(): lastSignal_(0) {}

	virtual void run();
	virtual void handleSignal(int signal);

private:
	friend class ThreadFactory;
	friend class Interrupt;
	friend class Process;

	static ThreadLocalRef<Thread> self_;
	Ref<ByteArray> stack_;
	pthread_t tid_;
	int lastSignal_;
};

inline Thread *thread() { return Thread::self(); }

} // namespace flux

#endif // FLUX_THREAD_H
