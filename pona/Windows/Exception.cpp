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

#include <windows.h>
#include <Winsock2.h>
#include <stdlib.h>

#include "../Exception.hpp"
#include "../LinePrinter.hpp"

#pragma warning(disable: 4996)

namespace pona
{

char* windowsError()
{
	DWORD err = GetLastError();

	wchar_t* msg;
	int result = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		err,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &msg,
		0, NULL
	);

	char* buf = 0;

	if (result > 0)
	{
		::free(buf);
		int bufLen = ::wcstombs(0, msg, 0) + 1;
		buf = (char*)::malloc(bufLen);
		::wcstombs(buf, msg, bufLen);
		buf[bufLen-1] = 0;
		LocalFree(msg);
	}
	else
	{
		String msg = format("GetLastError() = %%, WSAGetLastError() == %%") % err % WSAGetLastError();
		buf = msg.strdup();
	}

	return buf;
}

} // namespace pona
