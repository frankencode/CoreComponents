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

#ifndef PONA_STREAM_HPP
#define PONA_STREAM_HPP

#include "Atoms.hpp"

namespace pona
{

PONA_EXCEPTION(StreamException, Exception);
PONA_EXCEPTION(StreamSemanticException, StreamException);
PONA_EXCEPTION(StreamIoException, StreamException);
PONA_EXCEPTION(StreamEncodingException, StreamIoException);

class Stream: public Instance
{
public:
	virtual ~Stream() {}
	
	virtual bool isOpen() const = 0;
	virtual void close() = 0;
	
	virtual int readAvail(void* buf, int bufCapa) = 0;
	virtual void write(const void* buf, int bufFill) = 0;
	
	void read(void* buf, int bufCapa);
};

} // namespace pona

#endif // PONA_STREAM_HPP
