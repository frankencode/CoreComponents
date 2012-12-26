/*
 * Thread.hpp -- threads
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_THREAD_HPP
#define FTL_THREAD_HPP

#include <pthread.h>
#include <signal.h>
#include "atoms"
#include "Time.hpp"
#include "SignalSet.hpp"

namespace ftl
{

class ThreadFactory;
class Interrupt;

class Thread: public Action
{
public:
	enum DetachState {
		Joinable = PTHREAD_CREATE_JOINABLE,
		Detached = PTHREAD_CREATE_DETACHED
	};

	virtual ~Thread() {}

	static Ref<Thread> self();

	void start(int detachState = Joinable);
	void wait();
	void kill(int signal);
	bool stillAlive() const;

	static void sleep(Time duration);
	static void sleepUntil(Time timeout);

	static void blockAllSignals();
	static void blockSignal(int signal);
	static void unblockSignal(int signal);
	static void hookSignal(int signal);
	static void unhookSignal(int signal);

protected:
	friend class ThreadFactory;
	friend class Interrupt;

	Thread(): lastSignal_(0) {}

	virtual void run();
	virtual void handleSignal(int signal);

private:
	static void forwardSignal(int signal);
	static Ref<Thread, ThreadLocalOwner> self_;
	pthread_t tid_;
	int lastSignal_;
};

} // namespace ftl

#endif // FTL_THREAD_HPP
