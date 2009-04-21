#ifndef RGET_LINEFORWARDER_HPP
#define RGET_LINEFORWARDER_HPP

#include <pona/stdio>
#include <pona/thread>
#include <pona/event>
#include "Options.hpp"
#include "LogFile.hpp"

namespace rget
{

class LineForwarder: public Thread
{
public:
	LineForwarder(
		Ref<SystemStream> source,
		Ref<SystemStream> sink,
		String sourceEol,
		String sinkEol,
		Ref<LogFile> recvLog,
		Ref<EventManager> abortEvent
	);
	virtual int run();
	void abort();
	
private:
	Ref<SystemStream, Owner> source_, sink_;
	Ref<LineSource, Owner> lineSource_;
	Ref<LineSink, Owner> lineSink_;
	Ref<LogFile, Owner> recvLog_;
	Ref<EventManager, Owner> abortEvent_;
	Ref<EventHandler, Owner> abortHandler_;
	bool abort_;
};

} // namespace rget

#endif // RGET_LINEFORWARDER_HPP
