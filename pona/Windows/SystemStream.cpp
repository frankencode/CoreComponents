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

#include <Windows.h>
#include "../SystemStream.hpp"

namespace pona
{

SystemStream::SystemStream(void* handle, bool isStdHandle)
	: handle_(handle),
	  isStdHandle_(isStdHandle)
{
	isatty_ = (GetFileType(handle) == FILE_TYPE_CHAR);
}

SystemStream::~SystemStream()
{
	if ((!isStdHandle_) && isOpen())
		close();
}

int SystemStream::readAvail(void* buf, int bufCapa)
{
	DWORD bufFill;
	BOOL result = ReadFile(
		handle_,
		buf,
		bufCapa,
		&bufFill,
		/*lpOverlapped = */ 0
	);
	return (int)bufFill;
}

void SystemStream::write(const void* buf, int bufFill)
{
	DWORD numWritten;
	BOOL result = WriteFile(
		handle_,
		buf,
		bufFill,
		&numWritten,
		0 /*lpOverlapped*/
	);
	if (result == 0)
		PONA_THROW(StreamIoException, windowsError());
}

bool SystemStream::isOpen() const
{
	return (handle_ != INVALID_HANDLE_VALUE);
}

void SystemStream::close()
{
	if (CloseHandle(handle_) == 0)
		PONA_THROW(StreamSemanticException, windowsError());
	
	handle_ = INVALID_HANDLE_VALUE;
}

} // namespace pona
