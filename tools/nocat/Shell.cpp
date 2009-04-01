#include "CanonicalForwarder.hpp"
#include "Forwarder.hpp"
#include "Shell.hpp"

namespace nocat
{

Shell::Shell(Ref<Options> options)
	: options_(options)
{}

void Shell::session(Ref<Stream> stream)
{
	Ref<CanonicalForwarder, Owner> forwarder = new CanonicalForwarder(options_, stream);
	forwarder->start();
	
	Ref<LineSink, Owner> sink = new LineSink(stream);
	bool eoi = false;
	while (true) {
		String line = input()->readLine(&eoi);
		if (eoi) break;
		sink->writeLine(line);
	}
	
	forwarder->wait();
}

void Shell::transfer(Ref<Stream> stream)
{
	Ref<Forwarder, Owner> inputForwarder = new Forwarder(options_, rawInput(), stream);
	Ref<Forwarder, Owner> outputForwarder = new Forwarder(options_, stream, rawOutput());
	inputForwarder->start();
	outputForwarder->start();
	inputForwarder->wait();
	outputForwarder->wait();
}

} // namespace nocat
