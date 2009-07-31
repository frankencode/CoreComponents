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
#include "atoms"
#include "Time.hpp"

namespace pona
{

class Thread: public Instance
{
public:
	enum ExitType { Joinable, Detached };
	
	Thread();
	void start(int exitType = Joinable);
	int wait();
	
	static void sleep(Time duration);
	
	pthread_t tid() const;
	
protected:
	virtual int run() = 0;
	
private:
	pthread_t tid_;
	int exitCode_;
	bool keepAlive_;
	static void* runWrapper(void* p);
};

} // namespace pona

#endif // PONA_THREAD_HPP
