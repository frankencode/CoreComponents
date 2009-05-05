/*
 * LineForwarder.cpp -- canonical I/O transfer thread
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../../LICENSE for the license.
 */

#include "LineForwarder.hpp"

namespace rio
{

LineForwarder::LineForwarder(
	Ref<SystemStream> source,
	Ref<SystemStream> sink,
	String sourceEol,
	String sinkEol,
	Ref<LogFile> recvLog,
	Ref<Event> cancelEvent
)
	: source_(source),
	  sink_(sink),
	  lineSource_(new LineSource(source, options()->ioUnit_, sourceEol)),
	  lineSink_(new LineSink(sink, options()->ioUnit_, sinkEol)),
	  recvLog_(recvLog),
	  cancelEvent_(cancelEvent),
	  done_(false)
{
	class AbortAction: public Action {
	public:
		AbortAction(Ref<LineForwarder, Owner> forwarder)
			: forwarder_(forwarder)
		{}
		virtual void run() {
			forwarder_->finish();
		}
	private:
		Ref<LineForwarder, Owner> forwarder_;
	};
	
	finishAction_ = new AbortAction(this);
	cancelEvent_->pushFront(finishAction_);
}

void LineForwarder::finish() { done_ = true; }

int LineForwarder::run()
{
	int ret = 0;
	try {
		while (!done_)
		{
			if (source_->readyRead(1))
			{
				bool eoi = false;
				lineSource_->readAvail(&eoi);
				while (lineSource_->cachedLines() > 0) {
					String line = lineSource_->readLine();
					if (recvLog_) recvLog_->writeLine(line);
					lineSink_->writeLine(line);
				}
				if (eoi || done_) break;
			}
		}
	}
	catch (AnyException& ex) {
		printTo(error(), "(%%) LineForwarder: %%\n", options()->execName(), ex.what());
		ret = 1;
	}
	cancelEvent_->remove(finishAction_);
	cancelEvent_->run();
	finishAction_ = 0;
	return ret;
}

} // namespace rio
