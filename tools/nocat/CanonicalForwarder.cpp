#include <pona/stdio>
#include "CanonicalForwarder.hpp"

namespace nocat
{

CanonicalForwarder::CanonicalForwarder(Ref<Options> options, Ref<Stream> source)
	: options_(options),
	  source_(new LineSource(source)),
	  finished_(false)
{}

bool CanonicalForwarder::finished() const { return finished_; }

int CanonicalForwarder::run()
{
	int ret = 0;
	try {
		bool eoi = false;
		while (true) {
			String line = source_->readLine(&eoi);
			if (eoi) break;
			output()->writeLine(line);
		}
	}
	catch (std::Exception& ex) {
		print("(%%) %%\n", options_->toolName(), ex.what(), error());
		ret = 1;
	}
	print("CanonicalForwarder::run(): finished.\n");
	finished_ = true;
	// if (rawInput()->interactive()) { rawInput()->close(); print("Closed raw input.\n"); }
	return ret;
}

} // namespace nocat
