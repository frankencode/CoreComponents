/*
 * ProcessObserver.hpp -- waiting for process termination
 *
 * Copyright (c) 2007-2009, Frank Mertens
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
		: process_(process),
		  finishedEvent_(new Event)
	{}
	
	Ref<Event> finishedEvent() const { return finishedEvent_; }
	
private:
	int run() {
		int exitCode = process_->wait();
		finishedEvent_->run();
		return exitCode;
	}
	
	Ref<Process, Owner> process_;
	Ref<Event, Owner> finishedEvent_;
};

} // namespace rio

#endif // RIO_PROCESSOBSERVER_HPP
