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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> // sysconf
#include <pwd.h> // getpwuid_r
#include <grp.h> // getgrgid_r
#include "Exception.hpp"
#include "FileStatus.hpp"

namespace pona
{

void FileStatus::update()
{
	char* pathUtf8 = path_.strdup();
	struct stat status;
	int ret = ::stat(pathUtf8, &status);
	::free(pathUtf8);
	
	if (ret == -1) {
		exists_ = false;
		mode_ = 0;
		size_ = 0;
		owner_ = String();
		group_ = String();
	}
	else {
		exists_ = true;
		mode_ = status.st_mode;
		size_ = status.st_size;
		int maxLenPw = sysconf(_SC_GETPW_R_SIZE_MAX);
		int maxLenGr = sysconf(_SC_GETGR_R_SIZE_MAX);
		int bufLen = maxLenPw > maxLenGr ? maxLenPw : maxLenGr;
		char* buf = (char*)::malloc(bufLen);
		{
			struct passwd entry;
			struct passwd* pentry = 0;
			int ret = getpwuid_r(status.st_uid, &entry, buf, bufLen, &pentry);
			if (ret != 0)
				PONA_SYSTEM_EXCEPTION;
			if (pentry)
				owner_ = pentry->pw_name;
			else
				owner_ = "";
		}
		{
			struct group entry;
			struct group* pentry = 0;
			int ret = getgrgid_r(status.st_gid, &entry, buf, bufLen, &pentry);
			if (ret != 0)
				PONA_SYSTEM_EXCEPTION;
			if (pentry)
				group_ = pentry->gr_name;
			else
				group_ = "";
		}
		::free(buf);
	}
}

} // namespace pona
