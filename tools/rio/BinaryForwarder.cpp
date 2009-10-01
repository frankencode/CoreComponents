/*
 * BinaryForwarder.cpp -- binary I/O transfer thread
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../../LICENSE for the license.
 */

#include "BinaryForwarder.hpp"

namespace rio
{

BinaryForwarder::BinaryForwarder(Ref<SystemStream> source, Ref<SystemStream> sink, Ref<LogFile> recvLog, Ref<Event> cancelEvent)
	: source_(source),
	  sink_(sink),
	  recvLog_(recvLog),
	  cancelEvent_(cancelEvent),
	  done_(false), failed_(false),
	  buf_(options()->ioUnit_),
	  bytesTransferred_(0)
{
	class Finish: public Action {
	public:
		Finish(Ref<BinaryForwarder, Owner> forwarder)
			: forwarder_(forwarder)
		{}
		virtual void run() {
			forwarder_->finish();
		}
	private:
		Ref<BinaryForwarder, Owner> forwarder_;
	};
	
	finishAction_ = new Finish(this);
	cancelEvent_->pushBack(finishAction_);
}

void BinaryForwarder::finish() { done_ = true; }
uint64_t BinaryForwarder::bytesTransferred() const { return bytesTransferred_; }
bool BinaryForwarder::failed() const { return failed_; }

void BinaryForwarder::run()
{
	failed_ = false;
	try {
		while (!done_)
		{
			if (source_->readyRead(1)) {
				int fill = source_->readAvail(buf_, buf_.length());
				if ((fill == 0) || done_) break;
				recvLog_->write(buf_, fill);
				sink_->write(buf_, fill);
				bytesTransferred_ += fill;
			}
		}
	}
	catch (AnyException& ex) {
		printTo(error(), "(%%) %%\n", options()->execName(), ex.what());
		failed_ = true;
	}
	cancelEvent_->remove(finishAction_);
	cancelEvent_->run();
	finishAction_ = 0;
}

} // namespace rio
