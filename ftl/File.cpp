/*
 * File.cpp -- file I/O
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include <errno.h>
#include <string.h>
#include "ExitEvent.hpp"
#include "ThreadExitEvent.hpp"
#include "Guard.hpp"
#include "LocalStatic.hpp"
#include "Random.hpp"
#include "Path.hpp"
#include "Format.hpp"
#include "FileStatus.hpp"
#include "File.hpp"

namespace ftl
{

File::File(String path, int openFlags)
	: SystemStream(-1),
	  path_(path),
	  openFlags_(0),
	  unlinkWhenDone_(false)
{
	if (openFlags != 0)
		open(openFlags);
}

File::File(int fd)
	: SystemStream(fd),
	  openFlags_(0),
	  unlinkWhenDone_(false)
{
	if (fd == StandardInput)
		openFlags_ = Read;
	else if (fd == StandardOutput)
		openFlags_ = Write;
	else if (fd == StandardError)
		openFlags_ = Write;
	else
		FTL_THROW(StreamSemanticException, "Invalid argument");
}

File::~File()
{
	if (unlinkWhenDone_)
		try { unlink(); } catch(...) {}
}

String File::path() const
{
	return path_;
}

String File::name() const
{
	const char sep = '/';
	
	int n = path_->length();
	int i = n - 1;
	while (i >= 0) {
		if (path_->at(i) == sep) {
			++i;
			break;
		}
		--i;
	}
	
	String name;
	if (i < n)
		name = path_->copy(i, n);
	
	return name;
}

int File::openFlags() const
{
	return openFlags_;
}

bool File::access(int flags) const
{
	return isOpen() ? ((openFlags_ & flags) == flags) : (::access(path_, flags) == 0);
}

bool File::exists() const
{
	return access(Exists) && (path_->length() > 0);
}

void File::create(int mode)
{
	int fd = ::open(path_, O_RDONLY|O_CREAT|O_EXCL, mode);
	if (fd == -1)
		FTL_THROW(StreamSemanticException, systemError());
	::close(fd);
}

void File::unlink()
{
	if (::unlink(path_) == -1)
		FTL_THROW(StreamSemanticException, systemError());
}

void File::createUnique(int mode, char placeHolder)
{
	Random random;
	while (true) {
		String pathSaved = path_.copy();
		for (int i = 0, n = path_->size(); i < n; ++i) {
			if (path_->at(i) == placeHolder) {
				char r = random.get(0, 61);
				if ((0 <= r) && (r <= 9))
					r += '0';
				else if ((10 <= r) && (r <= 35))
					r += 'a' - 10;
				else if ((36 <= r) && (r <= 61))
					r += 'A' - 36;
				path_->set(i, r);
			}
		}
		int fd = ::open(path_, O_RDONLY|O_CREAT|O_EXCL, mode);
		if (fd == -1) {
			if (errno != EEXIST)
				FTL_THROW(StreamSemanticException, systemError());
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
			FTL_THROW(StreamSemanticException, systemError());
	}
	else {
		if (::truncate(path_, length) == -1)
			FTL_THROW(StreamSemanticException, systemError());
	}
}

class UnlinkFile: public Action {
public:
	UnlinkFile(String path): path_(Path(path).absolute()) {}
	void run() { try { File(path_).unlink(); } catch(...) {} }
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

void File::unlinkWhenDone() {
	unlinkWhenDone_ = true;
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
	fd_ = ::open(path_, h);
	if (fd_ == -1)
		FTL_THROW(StreamSemanticException, systemError());
	openFlags_ = flags;
}

Ref<File, Owner> File::open(String path, int flags)
{
	Ref<File, Owner> file = new File(path);
	file->open(flags);
	return file;
}

Ref<File, Owner> File::temp()
{
	Ref<File, Owner> file =
		new File(
			Format("/tmp/%%_%%_XXXXXXXX")
				<< Path(Process::execPath()).fileName()
				<< Process::currentId()
		);
	file->createUnique();
	file->open(Read|Write);
	return file;
}

String File::load(String path)
{
	return File::open(path, File::Read)->readAll();
}

void File::save(String path, String text)
{
	File::open(path, File::Write)->write(text);
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
		FTL_THROW(StreamSemanticException, systemError());
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

void File::sync()
{
	if (::fsync(fd_) == -1)
		FTL_THROW(StreamIoException, systemError());
}

void File::dataSync()
{
#if _POSIX_SYNCHRONIZED_IO > 0
	if (::fdatasync(fd_) == -1)
		FTL_THROW(StreamIoException, systemError());
#else
	sync();
#endif
}

void File::syncAll()
{
	::sync();
}

Ref<FileStatus> File::status() const
{
	if (!status_) {
		Mutex& mutex = localStatic<Mutex, FileStatus>();
		Guard<Mutex> guard(&mutex);
		if (!status_)
			status_ = new FileStatus(fd_);
	}
	return status_;
}

} // namespace ftl
