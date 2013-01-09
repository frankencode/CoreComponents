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

class Thread: public Instance
{
public:
	static Ref<Thread> self();

	void start();
	void wait();
	void kill(int signal);
	bool stillAlive() const;

	static void sleep(Time duration);
	static void sleepUntil(Time timeout);

	static void blockSignals(Ref<SignalSet> set);
	static void unblockSignals(Ref<SignalSet> set);

protected:
	Thread(): lastSignal_(0) {}

	virtual void run();
	virtual void handleSignal(int signal);

private:
	friend class ThreadFactory;
	friend class Interrupt;
	friend class Process;

	static Ref<Thread, ThreadLocalOwner> self_;
	pthread_t tid_;
	int lastSignal_;
};

} // namespace ftl

#endif // FTL_THREAD_HPP
