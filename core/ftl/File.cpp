 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */

#include <sys/mman.h> // mmap
#include <errno.h>
#include <string.h>
#include "PrintDebug.hpp" // DEBUG
#include "ExitEvent.hpp"
#include "ThreadExitEvent.hpp"
#include "Guard.hpp"
#include "Mutex.hpp"
#include "Random.hpp"
#include "format.hpp"
#include "Dir.hpp"
#include "Process.hpp"
#include "System.hpp"
#include "File.hpp"

namespace ftl
{

int File::translateOpenFlags(int openFlags)
{
	int h = 0;
	if (openFlags == Read)
		h = O_RDONLY;
	else if (openFlags == Write)
		h = O_WRONLY;
	else if (openFlags == (Read|Write))
		h = O_RDWR;
	return h;
}

hook<File> File::open(string path, int openFlags)
{
	int fd = ::open(path, translateOpenFlags(openFlags));
	if (fd == -1)
		FTL_SYSTEM_EXCEPTION;
	return new File(path, openFlags, fd);
}

hook<File> File::tryOpen(string path, int openFlags)
{
	int fd = ::open(path, translateOpenFlags(openFlags));
	if (fd != -1) return new File(path, openFlags, fd);
	return 0;
}

hook<File> File::open(int fd, int openFlags)
{
	return new File("", openFlags, fd);
}

hook<File> File::temp(int openFlags)
{
	string path = createUnique(
		format("/tmp/%%_%%_XXXXXXXX")
		<< Process::execPath()->fileName()
		<< Process::currentId()
	);
	if (path == "")
		FTL_SYSTEM_EXCEPTION;
	return open(path, openFlags);
}

File::File(string path, int openFlags, int fd)
	: SystemStream(fd),
	  path_(path),
	  openFlags_(openFlags),
	  unlinkWhenDone_(false)
{}

File::~File()
{
	if (unlinkWhenDone_)
		try { unlink(path_); } catch(...) {}
}

string File::path() const
{
	return path_;
}

string File::name() const
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

	string name;
	if (i < n)
		name = path_->copy(i, n);

	return name;
}

int File::openFlags() const
{
	return openFlags_;
}

hook<FileStatus> File::status() const
{
	return FileStatus::read(fd_);
}

void File::truncate(off_t length)
{
	if (isOpen()) {
		if (::ftruncate(fd_, length) == -1)
			FTL_SYSTEM_EXCEPTION;
	}
	else {
		if (::truncate(path_, length) == -1)
			FTL_SYSTEM_EXCEPTION;
	}
}

class UnlinkFile: public Action {
public:
	UnlinkFile(string path): path_(path->absolutePath()) {}
	void run() { try { unlink(path_); } catch(...) {} }
private:
	string path_;
};

void File::unlinkOnExit()
{
	exitEvent()->pushBack(new UnlinkFile(path_));
}

void File::unlinkOnThreadExit()
{
	threadExitEvent()->pushBack(new UnlinkFile(path_));
}

void File::unlinkWhenDone()
{
	unlinkWhenDone_ = true;
}

off_t File::seek(off_t distance, int method)
{
	off_t ret = ::lseek(fd_, distance, method);
	if (ret == -1) FTL_SYSTEM_EXCEPTION;
	return ret;
}

bool File::seekable() const
{
	return ::lseek(fd_, 0, SeekCurrent) != -1;
}

string File::map() const
{
	off_t fileSize = ::lseek(fd_, 0, SEEK_END);
	if (fileSize == -1)
		FTL_SYSTEM_EXCEPTION;
	if (fileSize == 0) return "";
	if (fileSize >= intMax) fileSize = intMax;
	int pageSize = System::pageSize();
	int protection =
		(PROT_READ * (openFlags_ & Read)) |
		(PROT_WRITE * (openFlags_ & Write)) |
		(PROT_EXEC * (openFlags_ & Execute));
	if (fileSize % pageSize != 0) {
		void *p = ::mmap(0, fileSize, protection, MAP_PRIVATE, fd_, 0);
		if (p == MAP_FAILED)
			FTL_SYSTEM_EXCEPTION;
		return string(hook<ByteArray>(new ByteArray((char*)p, fileSize, fileSize)));
	}
	else {
		#ifndef MAP_ANONYMOUS
		#define MAP_ANONYMOUS MAP_ANON
		#endif
		void *p = ::mmap(0, fileSize + pageSize, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (p == MAP_FAILED)
			FTL_SYSTEM_EXCEPTION;
		p = ::mmap(p, fileSize, protection, MAP_PRIVATE | MAP_FIXED, fd_, 0);
		if (p == MAP_FAILED)
			FTL_SYSTEM_EXCEPTION;
		return string(hook<ByteArray>(new ByteArray((char*)p, fileSize, fileSize + pageSize)));
	}
}

void File::sync()
{
	if (::fsync(fd_) == -1)
		FTL_SYSTEM_EXCEPTION;
}

void File::dataSync()
{
#if _POSIX_SYNCHRONIZED_IO > 0
	if (::fdatasync(fd_) == -1)
		FTL_SYSTEM_EXCEPTION;
#else
	sync();
#endif
}

bool File::access(string path, int flags)
{
	return ::access(path, flags) == 0;
}

bool File::exists(string path)
{
	return (path != "") && access(path, Exists);
}

bool File::create(string path, int mode)
{
	int fd = ::open(path, O_RDONLY|O_CREAT|O_EXCL, mode);
	if (fd == -1) return false;
	::close(fd);
	return true;
}

bool File::link(string path, string newPath)
{
	return ::link(path, newPath) != -1;
}

bool File::unlink(string path)
{
	return ::unlink(path) != -1;
}

bool File::symlink(string path, string newPath)
{
	return ::symlink(path, newPath) != -1;
}

string File::readlink(string path)
{
	string buf = string(128);
	while (true) {
		ssize_t numBytes = ::readlink(path, buf, buf->size());
		if (numBytes == -1)
			return string();
		if (numBytes <= buf->size()) {
			if (numBytes < buf->size())
				buf = string(buf->data(), numBytes);
			break;
		}
		buf = string(numBytes);
	}
	return buf;
}

string File::resolve(string path)
{
	string resolvedPath = path;
	while (File::unresolvedStatus(resolvedPath)->type() == File::Link) {
		string origPath = resolvedPath;
		resolvedPath = File::readlink(resolvedPath);
		if (resolvedPath == "") break;
		if (resolvedPath->isRelativePath())
			resolvedPath = origPath->reducePath()->expandPath(resolvedPath);
	}
	return resolvedPath;
}

string File::createUnique(string path, int mode, char placeHolder)
{
	hook<Random> random = Random::open();
	while (true) {
		string candidate = path->copy();
		for (int i = 0, n = candidate->size(); i < n; ++i) {
			if (candidate->at(i) == placeHolder) {
				char r = random->get(0, 61);
				if ((0 <= r) && (r <= 9))
					r += '0';
				else if ((10 <= r) && (r <= 35))
					r += 'a' - 10;
				else if ((36 <= r) && (r <= 61))
					r += 'A' - 36;
				candidate->set(i, r);
			}
		}
		int fd = ::open(candidate, O_RDONLY|O_CREAT|O_EXCL, mode);
		if (fd == -1) {
			if (errno != EEXIST)
				return "";
		}
		else {
			::close(fd);
			return candidate;
		}
	}
}

bool File::establish(string path, int fileMode, int dirMode)
{
	if (path->contains('/'))
		if (!Dir::establish(path->reducePath(), dirMode))
			return false;
	if (!File::exists(path))
		return File::create(path, fileMode);
	return true;
}


string File::lookup(string fileName, StringList *dirs, int accessFlags)
{
	hook<StringList> h;
	if (!dirs) {
		h = Process::env("PATH")->split(':');
		dirs = h;
	}
	string path;
	for (int i = 0; i < dirs->length(); ++i) {
		string candidate = format() << dirs->at(i) << "/" << fileName;
		if (access(candidate, accessFlags)) {
			path = candidate;
			break;
		}
	}
	return path;
}

hook<FileStatus> File::status(string path)
{
	return FileStatus::read(path, true);
}

hook<FileStatus> File::unresolvedStatus(string path)
{
	return FileStatus::read(path, false);
}

string File::load(string path)
{
	establish(path);
	return open(path, File::Read)->readAll();
}

void File::save(string path, string text)
{
	establish(path);
	hook<File> file = open(path, File::Write);
	file->truncate(0);
	file->write(text);
}

} // namespace ftl
