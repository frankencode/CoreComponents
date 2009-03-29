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
#include "FileStatus.hpp"

namespace pona
{

FileStatus::FileStatus(int fd)
	: fd_(fd)
{
	update();
}

FileStatus::FileStatus(String path)
	: fd_(-1),
	  path_(path)
{
	update();
}

void FileStatus::update(bool* exists)
{
	int ret = (fd_ != -1) ? ::fstat(fd_, this) : ::stat(path_.utf8(), this);
	if (ret == -1) {
		if (exists) {
			if ((errno == ENOENT) || (errno == ENOTDIR)) {
				*exists = false;
				return;
			}
		}
		PONA_SYSTEM_EXCEPTION;
	}
}

} // namespace pona
