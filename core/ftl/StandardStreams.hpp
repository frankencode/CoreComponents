 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_STANDARDSTREAMS_HPP
#define FTL_STANDARDSTREAMS_HPP

#include "atoms"
#include "SystemStream.hpp"
#include "LineSource.hpp"
#include "LineSink.hpp"
#include "ThreadLocalSingleton.hpp"

namespace ftl
{

class StandardStreams: public Instance, public ThreadLocalSingleton<StandardStreams>
{
public:
	inline SystemStream *rawInput() { return rawInput_; }
	inline SystemStream *rawOutput() { return rawOutput_; }
	inline SystemStream *rawError() { return rawError_; }

	inline LineSource *input() { return input_; }
	inline LineSink *output() { return output_; }
	inline LineSink *error() { return error_; }

private:
	friend class ThreadLocalSingleton<StandardStreams>;

	StandardStreams();

	Ref<SystemStream, Owner> rawInput_;
	Ref<SystemStream, Owner> rawOutput_;
	Ref<SystemStream, Owner> rawError_;
	Ref<LineSource, Owner> input_;
	Ref<LineSink, Owner> output_;
	Ref<LineSink, Owner> error_;
};

inline SystemStream *rawInput() { return StandardStreams::instance()->rawInput(); }
inline SystemStream *rawOutput() { return StandardStreams::instance()->rawOutput(); }
inline SystemStream *rawError() { return StandardStreams::instance()->rawError(); }

inline LineSource *input() { return StandardStreams::instance()->input(); }
inline LineSink *output() { return StandardStreams::instance()->output(); }
inline LineSink *error() { return StandardStreams::instance()->error(); }

} // namespace ftl

#endif // FTL_STANDARDSTREAMS_HPP
