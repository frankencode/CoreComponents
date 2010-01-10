/*
 * Thread.hpp -- threads
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_THREAD_HPP
#define PONA_THREAD_HPP

#include <pthread.h>
#include <signal.h> // SIGTERM, etc.
#include "atoms"
#include "Cloneable.hpp"
#include "Action.hpp"
#include "Time.hpp"

namespace pona
{

class ThreadFactory;

class Thread: public Cloneable, public Action
{
public:
	enum DetachState {
		Joinable = PTHREAD_CREATE_JOINABLE,
		Detached = PTHREAD_CREATE_DETACHED
	};
	
	void start(int detachState = Joinable);
	void wait();
	void kill(int signal = SIGTERM);
	
	bool started() const;
	bool isRunning() const;
	
	static void sleep(Time duration);
	static void sleepUntil(Time timeout);
	static void yield();
	
protected:
	friend class ThreadFactory;
	
	Thread();
	
	virtual Ref<Instance, Owner> clone();
	virtual void run() = 0;
	virtual void init();
	virtual void done();
	
	virtual void signalHandler(int signal);
	
	void enableSignal(int signal);
	void disableSignal(int signal);
	
private:
	static void signalForwarder(int signal);
	
	pthread_t tid_;
	bool keepAlive_;
	bool started_;
};

} // namespace pona

#endif // PONA_THREAD_HPP
