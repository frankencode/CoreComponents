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

namespace pona
{

class Thread: public Instance
{
public:
	enum ExitType { Joinable, Detached };

	Thread();
	~Thread();

	void start(int exitType = Joinable);
	int wait();

	static void sleep(TimeStamp dt);

protected:
	virtual int run() = 0;

private:
	pthread_t tid_;
	static void* runWrapper(void* p);
};

} // namespace pona

#endif // PONA_THREAD_HPP
