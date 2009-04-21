#include "LineForwarder.hpp"

namespace rget
{

LineForwarder::LineForwarder(
	Ref<SystemStream> source,
	Ref<SystemStream> sink,
	String sourceEol,
	String sinkEol,
	Ref<LogFile> recvLog,
	Ref<EventManager> abortEvent
)
	: source_(source),
	  sink_(sink),
	  lineSource_(new LineSource(source, options()->ioUnit_, sourceEol)),
	  lineSink_(new LineSink(sink, options()->ioUnit_, sinkEol)),
	  recvLog_(recvLog),
	  abortEvent_(abortEvent),
	  abort_(false)
{
	class AbortHandler: public EventHandler {
	public:
		AbortHandler(Ref<LineForwarder, Owner> forwarder)
			: forwarder_(forwarder)
		{}
		virtual void run() {
			print("(%%) LineForwarder::AbortHandler::run()\n", options()->execName());
			forwarder_->abort();
		}
	private:
		Ref<LineForwarder, Owner> forwarder_;
	};
	
	abortHandler_ = new AbortHandler(this);
	abortEvent->pushFront(abortHandler_);
}

void LineForwarder::abort() { abort_ = true; }

int LineForwarder::run()
{
	int ret = 0;
	try {
		while (!abort_)
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
				if (eoi || abort_) break;
			}
		}
	}
	catch (AnyException& ex) {
		printTo(error(), "(%%) LineForwarder: %%\n", options()->execName(), ex.what());
		ret = 1;
	}
	abortEvent_->remove(abortHandler_);
	abortEvent_->run(); // quick HACK
	return ret;
}

} // namespace rget
