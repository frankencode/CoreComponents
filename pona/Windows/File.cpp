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

#include <windows.h>
#include <stdlib.h>
#include "../File.hpp"
#include "../Exception.hpp"
#include "ProcessToken.hpp"

#pragma warning(disable: 4996)

namespace pona
{

File::File(String path)
	: SystemStream(INVALID_HANDLE_VALUE, false),
	  path_(path),
	  pathWc_(path_.wcsdup()),
	  openFlags_(0)
{}

File::File(int stdStream)
	: SystemStream(INVALID_HANDLE_VALUE, true),
	  pathWc_(0),
	  openFlags_(0)
{
	DWORD stdHandle = STD_OUTPUT_HANDLE;
	openFlags_ = 0;
	
	if (stdStream == stdInput)
	{
		stdHandle = STD_INPUT_HANDLE;
		openFlags_ = readable;
	}
	else if (stdStream == stdOutput)
	{
		stdHandle = STD_OUTPUT_HANDLE;
		openFlags_ = writable;
	}
	else if (stdStream == stdError)
	{
		stdHandle = STD_ERROR_HANDLE;
		openFlags_ = writable;
	}
	else
		PONA_THROW(StreamSemanticException, "Invalid argument");
	
	handle_ = GetStdHandle(stdHandle);
	if (handle_ == INVALID_HANDLE_VALUE)
		PONA_THROW(StreamSemanticException, windowsError());
}

File::~File()
{
	if (pathWc_) {
		free(pathWc_);
		pathWc_ = 0;
	}
}

String File::path() const
{
	return path_;
}

int File::openFlags() const
{
	return openFlags_;
}

bool File::access(int flags) const
{
	ProcessToken token;
	int rights = 0;

	PSECURITY_DESCRIPTOR sd;
	PACL dacl;
	DWORD ret = GetNamedSecurityInfo(
		/* pObjectName = */ pathWc_,
		/* ObjectType = */ SE_FILE_OBJECT,
		/* SecurityInfo =  */
		DACL_SECURITY_INFORMATION,
		0,
		0,
		&dacl,
		0,
		&sd
	);

	if (ret != ERROR_SUCCESS)
		return 0;
	
	{
		ACCESS_MASK mask;
		TRUSTEE trustee;
		ZeroMemory(&trustee, sizeof(trustee));
		trustee.TrusteeForm = TRUSTEE_IS_SID;
		trustee.TrusteeType = TRUSTEE_IS_USER;
		trustee.ptstrName = (LPTSTR)token.ownerSid();
		DWORD ret = GetEffectiveRightsFromAcl(dacl, &trustee, &mask);
		if (ret == ERROR_SUCCESS)
		{
			rights |= readable * ((mask & FILE_READ_DATA) != 0);
			rights |= writable * ((mask & FILE_WRITE_DATA) != 0);
			rights |= executable * ((mask & FILE_EXECUTE) != 0);
		}
	}

	{
		ACCESS_MASK mask;
		TRUSTEE trustee;
		ZeroMemory(&trustee, sizeof(trustee));
		trustee.TrusteeForm = TRUSTEE_IS_SID;
		trustee.TrusteeType = TRUSTEE_IS_GROUP;
		trustee.ptstrName = (LPTSTR)token.groupSid();
		DWORD ret = GetEffectiveRightsFromAcl(dacl, &trustee, &mask);
		if (ret == ERROR_SUCCESS)
		{
			rights |= readable * ((mask & FILE_READ_DATA) != 0);
			rights |= writable * ((mask & FILE_WRITE_DATA) != 0);
			rights |= executable * ((mask & FILE_EXECUTE) != 0);
		}
	}

	return (rights & flags) == flags;
}

bool File::exists() const
{
	HANDLE handle = CreateFile(
		pathWc_ /*lpFileName*/,
		GENERIC_READ /*dwDesiredAccess*/,
		0 /*dwShareMode*/,
		0 /*lpSecurityAttributes*/,
		OPEN_EXISTING /*dwCreationDisposition*/,
		FILE_FLAG_POSIX_SEMANTICS /*dwFlagsAndAttributes*/,
		0 /*hTemplateFile*/
	);

	bool ret = false;
	
	if (handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(handle);
		ret = true;
	}

	return ret;
}

void File::create(int mask)
{
	SECURITY_ATTRIBUTES sa;
	
	try { sa = ProcessToken().securityAttributes(mask); }
	catch (WindowsException&) { PONA_THROW(StreamSemanticException, windowsError()); }
	
	HANDLE handle = CreateFile(
		pathWc_ /*lpFileName*/,
		GENERIC_READ /*dwDesiredAccess*/,
		FILE_SHARE_WRITE|FILE_SHARE_READ|FILE_SHARE_DELETE /*dwShareMode*/,
		&sa /*lpSecurityAttributes*/,
		CREATE_NEW /*dwCreationDisposition*/,
		FILE_ATTRIBUTE_NORMAL|FILE_FLAG_POSIX_SEMANTICS /*dwFlagsAndAttributes*/,
		0 /*hTemplateFile*/
	);

	if (handle == INVALID_HANDLE_VALUE)
		PONA_THROW(StreamSemanticException, windowsError());
}

void File::remove()
{
	if (DeleteFile(pathWc_) == 0)
		PONA_THROW(StreamSemanticException, windowsError());
}

void File::open(int flags)
{
	DWORD access = 0;
	
	if (flags & readable)
		access |= GENERIC_READ;

	if (flags & writable)
		access |= GENERIC_WRITE;

	handle_ = CreateFile(
		pathWc_ /*lpFileName*/,
		access /*dwDesiredAccess*/,
		FILE_SHARE_WRITE|FILE_SHARE_READ|FILE_SHARE_DELETE /*dwShareMode*/,
		0 /*lpSecurityAttributes*/,
		OPEN_EXISTING /*dwCreationDisposition*/,
		FILE_FLAG_POSIX_SEMANTICS /*dwFlagsAndAttributes*/,
		0 /*hTemplateFile*/
	);

	if (handle_ == INVALID_HANDLE_VALUE)
		PONA_THROW(StreamSemanticException, windowsError());

	openFlags_ = flags;
}

File::Offset File::seek(Offset distance, int method)
{
	DWORD method2 = FILE_BEGIN;
	if (method == seekBegin)
		method2 = FILE_BEGIN;
	else if (method == seekCurrent)
		method2 = FILE_CURRENT;
	else if (method == seekEnd)
		method2 = FILE_END;

	LARGE_INTEGER h, h2;
	h.QuadPart = distance;
	BOOL result = SetFilePointerEx(
		handle_,
		h,
		&h2,
		method2
	);
	if (result == 0)
		PONA_THROW(StreamSemanticException, windowsError());

	return h2.QuadPart;
}

void File::seekSet(Offset distance)
{
	seek(distance, seekBegin);
}

void File::seekMove(Offset distance)
{
	seek(distance, seekCurrent);
}

File::Offset File::seekTell()
{
	return seek(0, seekCurrent);
}

File::Offset File::size()
{
	LARGE_INTEGER h;
	BOOL result = GetFileSizeEx(handle_, &h);

	if (result == 0)
		PONA_THROW(StreamSemanticException, windowsError());

	return h.QuadPart;
}

void File::sync()
{
	if (FlushFileBuffers(handle_) == 0)
		PONA_THROW(StreamIoException, windowsError());
}

void File::dataSync()
{
	sync();
}

} // namespace pona
