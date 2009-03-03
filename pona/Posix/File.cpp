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
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "../File.hpp"
#include "../Exception.hpp"

#include "../StandardStreams.hpp"
#include "../LinePrinter.hpp"

namespace pona
{

File::File(String path)
	: SystemStream(-1),
	  path_(path),
	  openFlags_(0),
	  pathUtf8_(path.strdup())
{}

File::File(int standardStream)
	: SystemStream(-1),
	  openFlags_(0),
	  pathUtf8_(0)
{
	if (standardStream == StandardInput) {
		fd_ = 0;
		openFlags_ = Read;
	}
	else if (standardStream == StandardOutput) {
		fd_ = 1;
		openFlags_ = Write;
	}
	else if (standardStream == StandardError) {
		fd_ = 2;
		openFlags_ = Write;
	}
	else
		PONA_THROW(StreamSemanticException, "Invalid argument");
}

File::~File()
{
	if (pathUtf8_) {
		free(pathUtf8_);
		pathUtf8_ = 0;
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
	int rights = 0;
	if (flags & Read) rights |= R_OK;
	if (flags & Write) rights |= W_OK;
	if (flags & Execute) rights |= X_OK;
	return ::access(pathUtf8_, rights) == 0;
}

bool File::exists() const
{
	return ::access(pathUtf8_, F_OK);
}

void File::create(int mask)
{
	int fd = ::open(pathUtf8_, O_RDONLY|O_CREAT|O_EXCL, mask);
	if (fd == -1)
		PONA_THROW(StreamSemanticException, posixError());
	::close(fd);
}

void File::unlink()
{
	if (::unlink(pathUtf8_) == -1)
		PONA_THROW(StreamSemanticException, posixError());
}

void File::open(int flags)
{
	int flags2 = 0;
	if (flags == Read)
		flags2 = O_RDONLY;
	else if (flags == Write)
		flags2 = O_WRONLY;
	else if (flags == (Read|Write))
		flags2 = O_RDWR;
	fd_ = ::open(pathUtf8_, flags2);
	if (fd_ == -1)
		PONA_THROW(StreamSemanticException, posixError());
}

File::Offset File::seek(Offset distance, int method)
{
	int method2 = SEEK_SET;
	if (method == SeekBegin)
		method2 = SEEK_SET;
	else if (method == SeekCurrent)
		method2 = SEEK_CUR;
	else if (method == SeekEnd)
		method2 = SEEK_END;
	File::Offset ret = ::lseek(fd_, distance, method2);
	if (ret == -1)
		PONA_THROW(StreamSemanticException, posixError());
	return ret;
}

void File::seekSet(Offset distance)
{
	seek(distance, SeekBegin);
}

void File::seekMove(Offset distance)
{
	seek(distance, SeekCurrent);
}

File::Offset File::seekTell()
{
	return seek(0, SeekCurrent);
}

File::Offset File::size()
{
	File::Offset h2 = seek(0, SeekCurrent);
	File::Offset h = seek(0, SeekEnd);
	seek(h2, SeekBegin);
	return h;
}

void File::sync()
{
	if (::fsync(fd_) == -1)
		PONA_THROW(StreamIoException, posixError());
}

void File::dataSync()
{
#if _POSIX_SYNCHRONIZED_IO > 0
	if (::fdatasync(fd_) == -1)
		PONA_THROW(StreamIoException, posixError());
#else
	sync();
#endif
}

} // namespace pona
