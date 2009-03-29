/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstraction Library.
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

#ifndef PONA_STANDARDSTREAMS_HPP
#define PONA_STANDARDSTREAMS_HPP

#include "atoms"
#include "Stream.hpp"
#include "LineSource.hpp"
#include "LineSink.hpp"
#include "ThreadLocalOwner.hpp"

namespace pona
{

class StandardStreams: public Instance
{
public:
	static Ref<StandardStreams> instance();

	inline Ref<Stream> rawInput() { return rawInput_; }
	inline Ref<Stream> rawOutput() { return rawOutput_; }
	inline Ref<Stream> rawError() { return rawError_; }

	inline Ref<LineSource> input() { return input_; }
	inline Ref<LineSink> output() { return output_; }
	inline Ref<LineSink> error() { return error_; }

private:
	StandardStreams();

	Ref<Stream, Owner> rawInput_;
	Ref<Stream, Owner> rawOutput_;
	Ref<Stream, Owner> rawError_;
	Ref<LineSource, Owner> input_;
	Ref<LineSink, Owner> output_;
	Ref<LineSink, Owner> error_;
};

inline Ref<Stream> rawInput() { return StandardStreams::instance()->rawInput(); }
inline Ref<Stream> rawOutput() { return StandardStreams::instance()->rawOutput(); }
inline Ref<Stream> rawError() { return StandardStreams::instance()->rawError(); }

inline Ref<LineSource> input() { return StandardStreams::instance()->input(); }
inline Ref<LineSink> output() { return StandardStreams::instance()->output(); }
inline Ref<LineSink> error() { return StandardStreams::instance()->error(); }

} // namespace pona

#endif // PONA_STANDARDSTREAMS_HPP
