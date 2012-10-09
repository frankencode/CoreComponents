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

File::File(String path, int openFlags)
	: SystemStream(-1),
	  path_(path),
	  openFlags_(0),
	  unlinkWhenDone_(false)
{
	if (openFlags != 0)
		open(openFlags);
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
	if (fd == -1) FTL_SYSTEM_EXCEPTION;
	::close(fd);
}

void File::link(const char *newPath)
{
	if (::link(path_, newPath) == -1)
		FTL_SYSTEM_EXCEPTION;
}

void File::unlink()
{
	if (::unlink(path_) == -1)
		FTL_SYSTEM_EXCEPTION;
}

bool File::link(const char *path, const char *newPath)
{
	return ::link(path, newPath) != -1;
}

bool File::unlink(const char *path)
{
	return ::unlink(path) != -1;
}

bool File::symlink(const char *path, const char *newPath)
{
	return ::symlink(path, newPath) != -1;
}

String File::readlink(const char *path)
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

String File::resolve(const char *path)
{
	String resolvedPath = path;
	while (FileStatus::newInstance(resolvedPath, false)->type() == File::SymbolicLink) {
		String origPath = resolvedPath;
		resolvedPath = File::readlink(resolvedPath);
		if (resolvedPath == "") break;
		if (resolvedPath->isRelativePath())
			resolvedPath = origPath->reducePath()->expandPath(resolvedPath);
	}
	return resolvedPath;
}

void File::createUnique(int mode, char placeHolder)
{
	Ref<Random, Owner> random = Random::newInstance();
	while (true) {
		String pathSaved = path_->copy();
		for (int i = 0, n = path_->size(); i < n; ++i) {
			if (path_->at(i) == placeHolder) {
				char r = random->get(0, 61);
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
				FTL_SYSTEM_EXCEPTION;
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
	void run() { try { File::newInstance(path_)->unlink(); } catch(...) {} }
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
		FTL_SYSTEM_EXCEPTION;
	openFlags_ = flags;
}

Ref<File, Owner> File::temp()
{
	Ref<File, Owner> file =
		new File(
			String(
				Format("/tmp/%%_%%_XXXXXXXX")
				<< Process::execPath()->fileName()
				<< Process::currentId()
			)
		);
	file->createUnique();
	file->open(Read|Write);
	return file;
}

void File::establish(int fileMode, int dirMode)
{
	if (path_->contains('/'))
		Dir::newInstance(path_->reducePath())->establish(dirMode);
	if (!exists())
		create(fileMode);
}

String File::load()
{
	String text;
	establish();
	open(File::Read);
	return readAll();
}

void File::save(String text)
{
	establish();
	open(File::Write);
	truncate(0);
	write(text);
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

void File::syncAll()
{
	::sync();
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
		if (File::newInstance(candidate)->access(accessFlags)) {
			path = candidate;
			break;
		}
	}
	return path;
}

Ref<FileStatus> File::status() const
{
	if (!status_) {
		if (isOpen())
			status_ = FileStatus::newInstance(fd_);
		else
			status_ = FileStatus::newInstance(path_);
	}
	return status_;
}

} // namespace ftl
