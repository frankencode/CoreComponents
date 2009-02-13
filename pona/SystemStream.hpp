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

#ifndef PONA_SYSTEMSTREAM_HPP
#define PONA_SYSTEMSTREAM_HPP

#include "Stream.hpp"

namespace pona
{

class SystemStream: public Stream
{
public:
#ifdef PONA_POSIX
	SystemStream(int fd);
	~SystemStream();
	inline int fd() const { return fd_; }
#else // if PONA_WINDOWS
	SystemStream(void* handle, bool isStdHandle = false);
	~SystemStream();
	inline void* handle() const { return handle_; }
#endif
	
	bool isOpen() const;
	void close();

	int readAvail(void* buf, int bufCapa);
	void write(const void* buf, int bufFill);

	bool isatty() const { return isatty_; }
	
protected:
	bool isatty_;
#ifdef PONA_POSIX
	int fd_;
#else // if PONA_WINDOWS
	void* handle_;
	bool isStdHandle_;
#endif
};

} // namespace pona

#endif // PONA_SYSTEMSTREAM_HPP

