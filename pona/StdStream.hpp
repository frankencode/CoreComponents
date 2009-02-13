/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstractions Library.
**
** Copyright (C) 2007-2009  Frank Mertens
**
** This file is part of a free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License as published
** by the Free Software Foundation, either version 3 of the License,
** or (at your option) any later version.
**
** The library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this libary.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef PONA_STDSTREAM_HPP
#define PONA_STDSTREAM_HPP

#include "Stream.hpp"
#include "LineSource.hpp"
#include "LineSink.hpp"

namespace pona
{

class StdStream: public Instance
{
public:
	static Ref<StdStream> instance();

	inline Ref<Stream> rawInput() { return rawInput_; }
	inline Ref<Stream> rawOutput() { return rawOutput_; }
	inline Ref<Stream> rawError() { return rawError_; }

	inline Ref<LineSource> input() { return input_; }
	inline Ref<LineSink> output() { return output_; }
	inline Ref<LineSink> error() { return error_; }

private:
	StdStream();

	Ref<Stream, Owner> rawInput_;
	Ref<Stream, Owner> rawOutput_;
	Ref<Stream, Owner> rawError_;
	Ref<LineSource, Owner> input_;
	Ref<LineSink, Owner> output_;
	Ref<LineSink, Owner> error_;
};

inline Ref<Stream> rawInput() { return StdStream::instance()->rawInput(); }
inline Ref<Stream> rawOutput() { return StdStream::instance()->rawOutput(); }
inline Ref<Stream> rawError() { return StdStream::instance()->rawError(); }

inline Ref<LineSource> input() { return StdStream::instance()->input(); }
inline Ref<LineSink> output() { return StdStream::instance()->output(); }
inline Ref<LineSink> error() { return StdStream::instance()->error(); }

} // namespace pona

#endif // PONA_STDSTREAM_HPP
