#ifndef RGET_PROCESSOBSERVER_HPP
#define RGET_PROCESSOBSERVER_HPP

#include <pona/thread>
#include <pona/context>
#include <pona/event>

namespace rget
{

class ProcessObserver: public Thread
{
public:
	ProcessObserver(Ref<Process> process)
		: process_(process),
		  finishedEvent_(new EventManager)
	{}
	
	Ref<EventManager> finishedEvent() const { return finishedEvent_; }
	
private:
	int run() {
		int exitCode = process_->wait();
		finishedEvent_->run();
		return exitCode;
	}
	
	Ref<Process, Owner> process_;
	Ref<EventManager, Owner> finishedEvent_;
};

} // namespace rget

#endif // RGET_PROCESSOBSERVER_HPP
