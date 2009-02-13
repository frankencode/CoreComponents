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

#include "Stream.hpp"

namespace pona
{

/*void Stream::write(const char* buf)
{
	int len = 0;
	while (buf[len] != 0) ++len;
	write(buf, len);
}*/

void Stream::read(void* buf, int bufCapa)
{
	uint8_t* buf2 = (uint8_t*)buf;
	int n = bufCapa;
	while (n > 0)
	{
		int dn = readAvail(buf2, n);
		if (dn == 0)
			PONA_THROW(StreamIoException, "Reading beyond end of input");
		buf2 += dn;
		n -= dn;
	}
}

} // namespace pona
