/*
 * LineForwarder.hpp -- canonical I/O transfer thread
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../../LICENSE for the license.
 */
#ifndef RIO_LINEFORWARDER_HPP
#define RIO_LINEFORWARDER_HPP

#include <pona/stdio>
#include <pona/threads>
#include <pona/events>
#include "Options.hpp"
#include "LogFile.hpp"

namespace rio
{

using namespace pona;

class LineForwarder: public Thread
{
public:
	LineForwarder(
		Ref<SystemStream> source,
		Ref<SystemStream> sink,
		UString sourceEol,
		UString sinkEol,
		Ref<LogFile> recvLog,
		Ref<Event> cancelEvent
	);
	
	virtual void run();
	bool failed() const;
	
	void finish();
	
private:
	Ref<SystemStream, Owner> source_, sink_;
	Ref<LineSource, Owner> lineSource_;
	Ref<LineSink, Owner> lineSink_;
	Ref<LogFile, Owner> recvLog_;
	Ref<Event, Owner> cancelEvent_;
	Ref<Action, Owner> finishAction_;
	bool done_, failed_;
};

} // namespace rio

#endif // RIO_LINEFORWARDER_HPP
