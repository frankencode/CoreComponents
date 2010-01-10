/*
 * StandardStreams.hpp -- thread local standard I/O streams
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_STANDARDSTREAMS_HPP
#define PONA_STANDARDSTREAMS_HPP

#include "atoms"
#include "SystemStream.hpp"
#include "LineSource.hpp"
#include "LineSink.hpp"
#include "ThreadLocalSingleton.hpp"

namespace pona
{

class StandardStreams: public Instance, public ThreadLocalSingleton<StandardStreams>
{
public:
	inline Ref<SystemStream> rawInput() { return rawInput_; }
	inline Ref<SystemStream> rawOutput() { return rawOutput_; }
	inline Ref<SystemStream> rawError() { return rawError_; }

	inline Ref<LineSource> input() { return input_; }
	inline Ref<LineSink> output() { return output_; }
	inline Ref<LineSink> error() { return error_; }

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

inline Ref<SystemStream> rawInput() { return StandardStreams::instance()->rawInput(); }
inline Ref<SystemStream> rawOutput() { return StandardStreams::instance()->rawOutput(); }
inline Ref<SystemStream> rawError() { return StandardStreams::instance()->rawError(); }

inline Ref<LineSource> input() { return StandardStreams::instance()->input(); }
inline Ref<LineSink> output() { return StandardStreams::instance()->output(); }
inline Ref<LineSink> error() { return StandardStreams::instance()->error(); }

} // namespace pona

#endif // PONA_STANDARDSTREAMS_HPP
