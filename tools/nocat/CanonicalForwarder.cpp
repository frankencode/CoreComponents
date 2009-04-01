#include "CanonicalForwarder.hpp"

namespace nocat
{

CanonicalForwarder::CanonicalForwarder(Ref<Options> options, Ref<Stream> source)
	: options_(options),
	  source_(new LineSource(source))
{}

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
	return ret;
}

} // namespace nocat
