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

#include <errno.h>
#include <string.h>
#include "Exception.hpp"

namespace pona
{

char* systemError()
{
#ifdef __USE_GNU
	const char* unknown = "Unknown error";
	const int bufSize = 1024;
	char buf[bufSize];
	::memcpy(buf, unknown, strlen(unknown) + 1);
	return ::strdup(::strerror_r(errno, buf, bufSize));
#else
	const char* unknown = "Unknown error";
	const int bufSize = 1024;
	char* buf = (char*)::malloc(bufSize);
	::memcpy(buf, unknown, strlen(unknown) + 1);
	int ret = ::strerror_r(errno, buf, bufSize);
	return buf;
#endif
}

} // namespace pona

