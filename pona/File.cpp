/*
 * File.cpp -- file I/O, status and permissions
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include <errno.h>
#include <string.h>
#include "ExitEvent.hpp"
#include "ThreadExitEvent.hpp"
#include "Random.hpp"
#include "Path.hpp"
#include "File.hpp"

namespace pona
{

File::File(String path)
	: SystemStream(-1),
	  path_(path),
	  openFlags_(0)
{}

File::File(int fd)
	: SystemStream(fd),
	  openFlags_(0)
{
	if (fd == StandardInput)
		openFlags_ = Read;
	else if (fd == StandardOutput)
		openFlags_ = Write;
	else if (fd == StandardError)
		openFlags_ = Write;
	else
		PONA_THROW(StreamSemanticException, "Invalid argument");
}

String File::path() const
{
	return path_;
}

String File::name() const
{
	Char sep = '/';
	
	int n = path_->length();
	int i = n - 1;
	while (i >= 0) {
		if (path_->get(i) == sep) {
			++i;
			break;
		}
		--i;
	}
	
	String name;
	if (i < n)
		name = path_->range(i, n - i);
	
	return name;
}

int File::openFlags() const
{
	return openFlags_;
}

bool File::access(int flags) const
{
	return isOpen() ? ((openFlags_ & flags) == flags) : ::access(path_.utf8(), flags) == 0;
}

bool File::exists() const
{
	return ::access(path_.utf8(), F_OK) == 0;
}

void File::create(int mask)
{
	int fd = ::open(path_.utf8(), O_RDONLY|O_CREAT|O_EXCL, mask);
	if (fd == -1)
		PONA_THROW(StreamSemanticException, systemError());
	::close(fd);
}

void File::unlink()
{
	if (::unlink(path_.utf8()) == -1)
		PONA_THROW(StreamSemanticException, systemError());
}

void File::createUnique(int mask, Char placeHolder)
{
	Random random;
	while (true) {
		String pathSaved = path_->copy();
		for (int i = 0, n = path_->length(); i < n; ++i) {
			if (path_->get(i) == placeHolder) {
				int r = random.get(0, 61);
				if ((0 <= r) && (r <= 9))
					path_->set(i, '0' + r);
				else if ((10 <= r) && (r <= 35))
					path_->set(i, 'a' + r - 10);
				else if ((36 <= r) && (r <= 61))
					path_->set(i, 'A' + r - 36);
			}
		}
		int fd = ::open(path_.utf8(), O_RDONLY|O_CREAT|O_EXCL, mask);
		if (fd == -1) {
			if (errno != EEXIST)
				PONA_THROW(StreamSemanticException, systemError());
			path_ = pathSaved;
		}
		else {
			::close(fd);
			break;
		}
	}
}

void File::truncate(off_t length)
{
	if (isOpen()) {
		if (::ftruncate(fd_, length) == -1)
			PONA_THROW(StreamSemanticException, systemError());
	}
	else {
		if (::truncate(path_.utf8(), length) == -1)
			PONA_THROW(StreamSemanticException, systemError());
	}
}

class UnlinkFile: public Action {
public:
	UnlinkFile(String path): path_(Path(path).makeAbsolute()) {}
	void run() { File(path_).unlink(); }
private:
	String path_;
};

void File::unlinkOnExit()
{
	exitEvent()->pushBack(new UnlinkFile(path_));
}

void File::unlinkOnThreadExit()
{
	threadExitEvent()->pushBack(new UnlinkFile(path_));
}

void File::open(int flags)
{
	int h = 0;
	if (flags == Read)
		h = O_RDONLY;
	else if (flags == Write)
		h = O_WRONLY;
	else if (flags == (Read|Write))
		h = O_RDWR;
	fd_ = ::open(path_.utf8(), h);
	if (fd_ == -1)
		PONA_THROW(StreamSemanticException, systemError());
	openFlags_ = flags;
}

off_t File::seek(off_t distance, int method)
{
	int method2 = SEEK_SET;
	if (method == SeekBegin)
		method2 = SEEK_SET;
	else if (method == SeekCurrent)
		method2 = SEEK_CUR;
	else if (method == SeekEnd)
		method2 = SEEK_END;
	off_t ret = ::lseek(fd_, distance, method2);
	if (ret == -1)
		PONA_THROW(StreamSemanticException, systemError());
	return ret;
}

void File::seekSet(off_t distance)
{
	seek(distance, SeekBegin);
}

void File::seekMove(off_t distance)
{
	seek(distance, SeekCurrent);
}

off_t File::seekTell()
{
	return seek(0, SeekCurrent);
}

off_t File::size()
{
	bool closeAgain = !isOpen();
	if (closeAgain) open(File::Read);
	off_t h2 = seek(0, SeekCurrent);
	off_t h = seek(0, SeekEnd);
	seek(h2, SeekBegin);
	if (closeAgain) close();
	return h;
}

Ref<FileStatus, Owner> File::status() const
{
	Ref<FileStatus, Owner> fs;
	if (isOpen())
		fs = new FileStatus(fd_);
	else if (exists())
		fs = new FileStatus(path_);
	return fs;
}

void File::sync()
{
	if (::fsync(fd_) == -1)
		PONA_THROW(StreamIoException, systemError());
}

void File::dataSync()
{
#if _POSIX_SYNCHRONIZED_IO > 0
	if (::fdatasync(fd_) == -1)
		PONA_THROW(StreamIoException, systemError());
#else
	sync();
#endif
}

} // namespace pona
