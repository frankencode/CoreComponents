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
	: rawInput_(File::open(File::StandardInput, File::Read)),
	  rawOutput_(File::open(File::StandardOutput, File::Write)),
	  rawError_(File::open(File::StandardError, File::Write)),
	  input_(LineSource::open(rawInput_)),
	  output_(LineSink::open(rawOutput_)),
	  error_(LineSink::open(rawError_))
{}

} // namespace ftl
