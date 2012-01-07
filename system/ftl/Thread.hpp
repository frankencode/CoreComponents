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
#include <signal.h> // SIGTERM, etc.
#include "atoms"
#include "Time.hpp"

namespace ftl
{

class ThreadFactory;

class Thread: public Action
{
public:
	enum DetachState {
		Joinable = PTHREAD_CREATE_JOINABLE,
		Detached = PTHREAD_CREATE_DETACHED
	};
	
	virtual ~Thread() {}
	
	void start(int detachState = Joinable);
	void wait();
	void kill(int signal = SIGUSR2);
	bool stillAlive() const;
	
	static void sleep(Time duration);
	static void sleepUntil(Time timeout);
	
	static void enableSignal(int signal = SIGUSR2);
	static void disableSignal(int signal = SIGUSR2);
	
protected:
	friend class ThreadFactory;
	
	virtual void run() = 0;
	
private:
	static void forwardSignal(int signal);
	
	pthread_t tid_;
};

} // namespace ftl

#endif // FTL_THREAD_HPP
