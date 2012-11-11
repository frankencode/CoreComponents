/*
 * File.cpp -- file I/O
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include <errno.h>
#include <string.h>
#include "ExitEvent.hpp"
#include "ThreadExitEvent.hpp"
#include "Guard.hpp"
#include "Mutex.hpp"
#include "Random.hpp"
#include "Format.hpp"
#include "Dir.hpp"
#include "Process.hpp"
#include "File.hpp"

namespace ftl
{

bool File::access(String path, int flags)
{
	return ::access(path, flags) == 0;
}

bool File::exists(String path)
{
	return (path != "") && access(path, Exists);
}

bool File::create(String path, int mode)
{
	int fd = ::open(path, O_RDONLY|O_CREAT|O_EXCL, mode);
	if (fd == -1) return false;
	::close(fd);
	return true;
}

bool File::link(String path, String newPath)
{
	return ::link(path, newPath) != -1;
}

bool File::unlink(String path)
{
	return ::unlink(path) != -1;
}

bool File::symlink(String path, String newPath)
{
	return ::symlink(path, newPath) != -1;
}

String File::readlink(String path)
{
	String buf = String(128);
	while (true) {
		ssize_t numBytes = ::readlink(path, buf, buf->size());
		if (numBytes == -1)
			return String();
		if (numBytes <= buf->size()) {
			if (numBytes < buf->size())
				buf = String(buf->data(), numBytes);
			break;
		}
		buf = String(numBytes);
	}
	return buf;
}

String File::resolve(String path)
{
	String resolvedPath = path;
	while (File::unresolvedStatus(resolvedPath)->type() == File::Link) {
		String origPath = resolvedPath;
		resolvedPath = File::readlink(resolvedPath);
		if (resolvedPath == "") break;
		if (resolvedPath->isRelativePath())
			resolvedPath = origPath->reducePath()->expandPath(resolvedPath);
	}
	return resolvedPath;
}

String File::createUnique(String path, int mode, char placeHolder)
{
	Ref<Random, Owner> random = Random::open();
	while (true) {
		String candidate = path->copy();
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

bool File::establish(String path, int fileMode, int dirMode)
{
	if (path->contains('/'))
		if (!Dir::establish(path->reducePath(), dirMode))
			return false;
	if (!File::exists(path))
		return File::create(path, fileMode);
	return true;
}


String File::lookup(String fileName, Ref<StringList> dirs, int accessFlags)
{
	Ref<StringList, Owner> h;
	if (!dirs) {
		h = Process::env("PATH")->split(':');
		dirs = h;
	}
	String path;
	for (int i = 0; i < dirs->length(); ++i) {
		String candidate = Format() << dirs->at(i) << "/" << fileName;
		if (access(candidate, accessFlags)) {
			path = candidate;
			break;
		}
	}
	return path;
}

Ref<FileStatus, Owner> File::status(String path)
{
	return FileStatus::newInstance(path, true);
}

Ref<FileStatus, Owner> File::unresolvedStatus(String path)
{
	return FileStatus::newInstance(path, false);
}

String File::load(String path)
{
	establish(path);
	return open(path, File::Read)->readAll();
}

void File::save(String path, String text)
{
	establish(path);
	Ref<File, Owner> file = open(path, File::Write);
	file->truncate(0);
	file->write(text);
}

Ref<File, Owner> File::temp()
{
	String path = createUnique(
		Format("/tmp/%%_%%_XXXXXXXX")
		<< Process::execPath()->fileName()
		<< Process::currentId()
	);
	if (path == "")
		FTL_SYSTEM_EXCEPTION;
	return open(path);
}

File::File(String path, int openFlags)
	: SystemStream(-1),
	  path_(path),
	  openFlags_(openFlags),
	  unlinkWhenDone_(false)
{
	int h = 0;
	if (openFlags == Read)
		h = O_RDONLY;
	else if (openFlags == Write)
		h = O_WRONLY;
	else if (openFlags == (Read|Write))
		h = O_RDWR;
	fd_ = ::open(path_, h);
	if (fd_ == -1)
		FTL_SYSTEM_EXCEPTION;
}

File::File(int fd, int openFlags)
	: SystemStream(fd),
	  openFlags_(openFlags),
	  unlinkWhenDone_(false)
{
	if (fd == StandardInput)
		openFlags_ = Read;
	else if (fd == StandardOutput)
		openFlags_ = Write;
	else if (fd == StandardError)
		openFlags_ = Write;
}

File::~File()
{
	if (unlinkWhenDone_)
		try { unlink(path_); } catch(...) {}
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

Ref<FileStatus, Owner> File::status() const
{
	return FileStatus::newInstance(fd_);
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
	UnlinkFile(String path): path_(path->makeAbsolutePath()) {}
	void run() { try { unlink(path_); } catch(...) {} }
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

} // namespace ftl
