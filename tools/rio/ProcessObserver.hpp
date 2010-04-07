/*
 * ProcessObserver.hpp -- waiting for process termination
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../../LICENSE for the license.
 */
#ifndef RIO_PROCESSOBSERVER_HPP
#define RIO_PROCESSOBSERVER_HPP

#include <pona/threads>
#include <pona/process>
#include <pona/events>

namespace rio
{

using namespace pona;

class ProcessObserver: public Thread
{
public:
	ProcessObserver(Ref<Process> process)
		: process_(process.get()),
		  exitCode_(0),
		  finishedEvent_(new Event)
	{}
	
	Ref<Event> finishedEvent() const { return finishedEvent_; }
	int exitCode() const { return exitCode_; }
	
private:
	void run() {
		exitCode_ = process_->wait();
		finishedEvent_->run();
	}
	
	Ref<Process, Owner> process_;
	int exitCode_;
	Ref<Event, Owner> finishedEvent_;
};

} // namespace rio

#endif // RIO_PROCESSOBSERVER_HPP
