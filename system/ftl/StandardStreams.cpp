/*
 * StandardStreams.cpp -- thread local standard I/O streams
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include "defaults.hpp"
#include "Mutex.hpp"
#include "File.hpp"
#include "StandardStreams.hpp"

namespace ftl
{

StandardStreams::StandardStreams()
	: rawInput_(File::newInstance(File::StandardInput)),
	  rawOutput_(File::newInstance(File::StandardOutput)),
	  rawError_(File::newInstance(File::StandardError)),
	  input_(LineSource::newInstance(rawInput_)),
	  output_(LineSink::newInstance(rawOutput_)),
	  error_(LineSink::newInstance(rawError_))
{}

} // namespace ftl
