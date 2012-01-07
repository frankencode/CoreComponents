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
	: rawInput_(new File(File::StandardInput)),
	  rawOutput_(new File(File::StandardOutput)),
	  rawError_(new File(File::StandardError)),
	  input_(new LineSource(rawInput_)),
	  output_(new LineSink(rawOutput_)),
	  error_(new LineSink(rawError_))
{}

} // namespace ftl
