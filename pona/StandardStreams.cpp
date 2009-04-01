/*
 * StandardStreams.cpp -- thread local standard I/O streams
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "defaults.hpp"
#include "Mutex.hpp"
#include "File.hpp"
#include "StandardStreams.hpp"

namespace pona
{

Ref<StandardStreams> StandardStreams::instance()
{
	static Ref<StandardStreams, ThreadLocalOwner> instance_ = 0;
	
	if (!instance_)
		instance_ = new StandardStreams();
	return instance_;
}

StandardStreams::StandardStreams()
	: rawInput_(new File(File::StandardInput)),
	  rawOutput_(new File(File::StandardOutput)),
	  rawError_(new File(File::StandardError)),
	  input_(new LineSource(rawInput_)),
	  output_(new LineSink(rawOutput_)),
	  error_(new LineSink(rawError_))
{}

} // namespace pona
