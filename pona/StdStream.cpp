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

#include "Options.hpp"
#include "Mutex.hpp"
#include "File.hpp"
#include "StdStream.hpp"

namespace pona
{

Ref<StdStream> StdStream::instance()
{
	static Ref<StdStream, ThreadLocalOwner> instance_ = 0;
	
	if (instance_ == 0)
		instance_ = new StdStream();
	return instance_;
}

StdStream::StdStream()
	: rawInput_(new File(File::stdInput)),
	  rawOutput_(new File(File::stdOutput)),
	  rawError_(new File(File::stdError)),
	  input_(new LineSource(rawInput_)),
	  output_(new LineSink(rawOutput_)),
	  error_(new LineSink(rawError_))
{}

} // namespace pona
