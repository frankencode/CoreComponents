#ifndef NOCAT_CANONICALFORWARDER_HPP
#define NOCAT_CANONICALFORWARDER_HPP

#include <pona/stdio>
#include <pona/threads>
#include "Options.hpp"

namespace nocat
{

class CanonicalForwarder: public Thread
{
public:
	CanonicalForwarder(Ref<Options> options, Ref<Stream> source);
	virtual int run();
	
private:
	Ref<Options, Owner> options_;
	Ref<LineSource, Owner> source_;
};

} // namespace nocat

#endif // NOCAT_CANONICALFORWARDER_HPP
