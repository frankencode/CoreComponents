#ifndef NOCAT_FORWARDER_HPP
#define NOCAT_FORWARDER_HPP

#include <pona/stdio>
#include <pona/thread>
#include "Options.hpp"

namespace nocat
{

using namespace pona;

class Forwarder: public Thread
{
public:
	Forwarder(Ref<Options> options, Ref<Stream> source, Ref<Stream> sink);
	virtual int run();
	uint64_t bytesTransferred() const;
	
private:
	Ref<Options, Owner> options_;
	Ref<Stream, Owner> source_, sink_;
	Vector<uint8_t> buf_;
	uint64_t bytesTransferred_;
};

} // namespace nocat

#endif // NOCAT_FORWARDER_HPP
