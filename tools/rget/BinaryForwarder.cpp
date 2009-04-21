#include "BinaryForwarder.hpp"

namespace rget
{

BinaryForwarder::BinaryForwarder(Ref<SystemStream> source, Ref<SystemStream> sink, Ref<LogFile> recvLog, Ref<EventManager> abortEvent)
	: source_(source),
	  sink_(sink),
	  recvLog_(recvLog),
	  abort_(false),
	  abortEvent_(abortEvent),
	  buf_(options()->ioUnit_),
	  bytesTransferred_(0)
{
	class AbortHandler: public EventHandler {
	public:
		AbortHandler(Ref<BinaryForwarder, Owner> forwarder)
			: forwarder_(forwarder)
		{}
		virtual void run() {
			print("(%%) BinaryForwarder::AbortHandler::run()\n", options()->execName());
			forwarder_->abort();
		}
	private:
		Ref<BinaryForwarder, Owner> forwarder_;
	};
	
	abortHandler_ = new AbortHandler(this);
	abortEvent->pushFront(abortHandler_);
}

BinaryForwarder::~BinaryForwarder()
{
	print("(%%) ~BinaryForwarder()\n", options()->execName());
}

int BinaryForwarder::run()
{
	int ret = 0;
	try {
		while (!abort_)
		{
			if (source_->readyRead(1)) {
				int fill = source_->readAvail(buf_, buf_.length());
				if ((fill == 0) || abort_) break;
				recvLog_->write(buf_, fill);
				sink_->write(buf_, fill);
				bytesTransferred_ += fill;
			}
		}
	}
	catch (AnyException& ex) {
		printTo(error(), "(%%) %%\n", options()->execName(), ex.what());
		ret = 1;
	}
	print("(%%) BinaryForwarder::run(): finished\n", options()->execName());
	abortEvent_->remove(abortHandler_);
	abortHandler_ = 0;
	// abortEvent_->run();
	return ret;
}

void BinaryForwarder::abort() { abort_ = true; }
uint64_t BinaryForwarder::bytesTransferred() const { return bytesTransferred_; }

} // namespace rget
