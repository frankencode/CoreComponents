#include "Forwarder.hpp"

namespace nocat
{

Forwarder::Forwarder(Ref<Options> options, Ref<Stream> source, Ref<Stream> sink)
	: options_(options),
	  source_(source),
	  sink_(sink),
	  buf_(options->ioUnit_)
{}

int Forwarder::run()
{
	int ret = 0;
	try {
		while (true) {
			int fill = source_->readAvail(buf_, buf_.length());
			if (!fill) break;
			sink_->write(buf_, fill);
			bytesTransferred_ += fill;
		}
	}
	catch (std::Exception& ex) {
		print("(%%) %%\n", options_->toolName(), ex.what(), error());
		ret = 1;
	}
	return ret;
}

uint64_t Forwarder::bytesTransferred() const { return bytesTransferred_; }

} // namespace nocat
