/*
 * Thread.hpp -- threads
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_THREAD_HPP
#define PONA_THREAD_HPP

#include <pthread.h>
#include <signal.h> // pthread_kill, SIGTERM
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
		Detached =  PTHREAD_CREATE_DETACHED
	};
	
	void start(int detachState = Joinable);
	void wait();
	
	void kill(int signal = SIGTERM);
	
	static void sleep(Time duration);
	
protected:
	friend class ThreadFactory;
	Thread();
	
	virtual Ref<Instance, Owner> clone();
	virtual void run() = 0;
	virtual void init();
	virtual void done();
	
private:
	pthread_t tid_;
	bool keepAlive_;
};

} // namespace pona

#endif // PONA_THREAD_HPP
