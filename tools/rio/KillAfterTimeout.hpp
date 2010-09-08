/*
 * KillAfterTimeout.hpp -- terminate process after timeout
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../../LICENSE for the license.
 */
#ifndef RIO_KILLAFTERTIMEOUT_HPP
#define RIO_KILLAFTERTIMEOUT_HPP

#include <pona/stdio>
#include <pona/threads>
#include <pona/process>
#include <pona/time>

namespace rio
{

class KillAfterTimeout: public Thread
{
public:
	KillAfterTimeout(Ref<Process> process, Time timeout, int signal)
		: processId_(process->processId()),
		  groupLeader_(process->type() == Process::GroupLeader),
		  timeout_(timeout),
		  signal_(signal)
	{}
	
	void abort() {
		mutex_.acquire();
		abort_.signal();
		mutex_.release();
	}
	
	void run() {
		mutex_.acquire();
		if (!abort_.waitUntil(&mutex_, timeout_)) {
			try {
				if (groupLeader_) Process::killGroup(processId_, signal_);
				else Process::kill(processId_, signal_);
			}
			catch (...) {}
		}
		mutex_.release();
	}
	
private:
	Mutex mutex_;
	Condition abort_;
	pid_t processId_;
	bool groupLeader_;
	Time timeout_;
	int signal_;
	
};

} // namespace rio

#endif // RIO_KILLAFTERTIMEOUT_HPP
