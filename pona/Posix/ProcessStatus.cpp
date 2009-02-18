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

#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include "../Exception.hpp"
#include "../ProcessStatus.hpp"

extern "C" char** environ;

namespace pona
{

/** return the working directory of the calling process
  */
String ProcessStatus::workingDirectory()
{
	size_t bufSize = 4096;
	char* buf = (char*)malloc(bufSize);
	while (::getcwd(buf, bufSize) == 0)
	{
		free(buf);
		bufSize += 4096;
		buf = (char*)malloc(bufSize);
	}
	String s(buf);
	free(buf);
	return s;
}

/** return the effective user name of the calling process
  */
String ProcessStatus::userName()
{
	return ::getpwuid(geteuid())->pw_name;
}

/** return the effective group name of the calling process
  */
String ProcessStatus::groupName()
{
	return ::getgrgid(getegid())->gr_name;
}

/** returns true, if the calling process is running with super user permissions
  */
bool ProcessStatus::isSuperUser()
{
	return (::geteuid() == 0) || (::getegid() == 0);
}

/** return the environment of the calling process
  */
Ref<ProcessStatus::Environment, Owner> ProcessStatus::environment()
{
	char** env = ::environ;
	Ref<Environment, Owner> map = new Environment;
	int i = 0;
	while (env[i] != 0)
	{
		String s(env[i]);
		int k = s.find("=");
		if (k != -1)
		{
			String key = s.copy(0, k);
			String value = s.copy(k + 1, s.length() - (k + 1));
			map->set(key, value);
		}
		++i;
	}
	return map;
}

void ProcessStatus::setEnv(String key, String value)
{
	char* keyUtf8 = key.strdup();
	char* valueUtf8 = value.strdup();
	
	if (setenv(keyUtf8, valueUtf8, 1) == -1)
		PONA_POSIX_EXCEPTION;
	
	::free(keyUtf8);
	::free(valueUtf8);
}

String ProcessStatus::env(String key)
{
	char* keyUtf8 = key.strdup();
	String value(getenv(keyUtf8));
	::free(keyUtf8);
	return value;
}

} // namespace pona
