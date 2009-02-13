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
#include "ProcessToken.hpp"
#include "../Exception.hpp"
#include "../ProcessStatus.hpp"

namespace pona
{

String ProcessStatus::workingDirectory()
{
	String path;
	int bufSize = 1024;

	while (true)
	{
		wchar_t* buf = new wchar_t[bufSize];
		DWORD ret = GetCurrentDirectory(bufSize, buf);
		if (ret == 0)
			PONA_WINDOWS_EXCEPTION;
		if (int(ret) > bufSize) {
			delete[] buf;
			bufSize = ret + 1;
			continue;
		}
		path = String(buf);
		delete[] buf;
		break;
	}

	return path;
}

String ProcessStatus::userName()
{
	return ProcessToken().owner();
}

String ProcessStatus::groupName()
{
	return ProcessToken().group();
}

bool ProcessStatus::isSuperUser()
{
	return true;
}

Ref<ProcessStatus::Environment, Owner> ProcessStatus::environment()
{
	wchar_t* env = GetEnvironmentStrings();
	if (!env)
		PONA_WINDOWS_EXCEPTION;
	Ref<Environment, Owner> map = new Environment;
	while (*env)
	{
		String s(env);
		int k = s.find("=");
		if (k != -1)
		{
			String key = s.copy(0, k);
			String value = s.copy(k + 1, s.length() - (k + 1));
			map->set(key, value);
		}
		env += s.length() + 1;
	}
	if (!FreeEnvironmentStrings(env))
		PONA_WINDOWS_EXCEPTION;
	return map;
}

void ProcessStatus::setEnv(String key, String value)
{
	wchar_t* keyWc = key.wcsdup();
	wchar_t* valueWc = value.wcsdup();
	if (!SetEnvironmentVariable(keyWc, valueWc))
		PONA_WINDOWS_EXCEPTION;
	::free(keyWc);
	::free(valueWc);
}

String ProcessStatus::env(String key)
{
	wchar_t* keyWc = key.wcsdup();
	const int bufSize = 32768;
	wchar_t* buf = (wchar_t*)::malloc(bufSize);
	String s;
	DWORD fill = GetEnvironmentVariable(keyWc, buf, bufSize);
	if (fill > 0) {
		buf[fill] = 0;
		s = buf;
	}
	else if (GetLastError() != ERROR_ENVVAR_NOT_FOUND) {
		PONA_WINDOWS_EXCEPTION;
	}
	::free(buf);
	::free(keyWc);
	return s;
}

} // namespace pona
