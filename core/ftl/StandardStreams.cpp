 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
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
