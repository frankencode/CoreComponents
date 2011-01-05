/*
 * Thread.hpp -- threads
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
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
	
	static void enableSignal(int signal);
	static void disableSignal(int signal);
	
protected:
	friend class ThreadFactory;
	
	virtual void run() = 0;
	
private:
	static void forwardSignal(int signal);
	
	pthread_t tid_;
};

} // namespace ftl

#endif // FTL_THREAD_HPP
