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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include "../FileStatus.hpp"

namespace pona
{

void FileStatus::update()
{
	char* pathUtf8 = path_.strdup();
	struct stat buf;
	int ret = stat(pathUtf8, &buf);
	free(pathUtf8);
	
	if (ret == -1)
	{
		exists_ = false;
		mode_ = 0;
		size_ = 0;
		owner_ = String();
		group_ = String();
	}
	else
	{
		exists_ = true;
		mode_ = buf.st_mode;
		size_ = buf.st_size;
		owner_ = String(getpwuid(buf.st_uid)->pw_name);
		group_ = String(getpwuid(buf.st_gid)->pw_name);
	}
}

} // namespace pona
