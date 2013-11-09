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
#include <stdio.h> // rename
#include "ExitEvent.h"
#include "ThreadExitEvent.h"
#include "Guard.h"
#include "Mutex.h"
#include "Random.h"
#include "Format.h"
#include "Dir.h"
#include "Process.h"
#include "System.h"
#include "File.h"

namespace flux
{

Ref<File> File::open(String path, int openFlags)
{
	int fd = ::open(path, openFlags);
	if (fd == -1)
		FLUX_SYSTEM_EXCEPTION;
	return new File(path, openFlags, fd);
}

Ref<File> File::tryOpen(String path, int openFlags)
{
	int fd = ::open(path, openFlags);
	if (fd != -1) return new File(path, openFlags, fd);
	return 0;
}

Ref<File> File::open(int fd, int openFlags)
{
	return new File("", openFlags, fd);
}

Ref<File> File::temp(int openFlags)
{
	String path = createUnique(
		Format("/tmp/%%_%%_XXXXXXXX")
		<< Process::execPath()->fileName()
		<< Process::currentId()
	);
	if (path == "")
		FLUX_SYSTEM_EXCEPTION;
	return open(path, openFlags);
}

File::File(String path, int openFlags, int fd)
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

String File::path() const
{
	return path_;
}

String File::name() const
{
	const char sep = '/';

	int n = path_->size();
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

Ref<FileStatus> File::status() const
{
	return FileStatus::read(fd_);
}

void File::truncate(off_t length)
{
	if (isOpen()) {
		if (::ftruncate(fd_, length) == -1)
			FLUX_SYSTEM_EXCEPTION;
	}
	else {
		if (::truncate(path_, length) == -1)
			FLUX_SYSTEM_EXCEPTION;
	}
}

class UnlinkFile: public Action {
public:
	UnlinkFile(String path): path_(path->absolutePath()) {}
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

void File::unlinkWhenDone()
{
	unlinkWhenDone_ = true;
}

off_t File::seek(off_t distance, int method)
{
	off_t ret = ::lseek(fd_, distance, method);
	if (ret == -1) FLUX_SYSTEM_EXCEPTION;
	return ret;
}

bool File::seekable() const
{
	return ::lseek(fd_, 0, SeekCurrent) != -1;
}

String File::map(off_t i0, off_t i1) const
{
	if (i0 < 0) i0 = 0;
	if (i1 < i0) {
		i1 = ::lseek(fd_, 0, SEEK_END);
		if (i1 == -1) FLUX_SYSTEM_EXCEPTION;
	}

	if (i1 == 0) return String();

	const int np = System::pageSize();

	int nt = 0;
	if (i0 > 0) {
		nt = i0 % np;
		i0 -= nt;
	}

	size_t n = i1 - i0;
	if (n >= off_t(intMax)) n = intMax - 1;

	size_t m = (n % np == 0) ? n + np : n;

	#ifndef MAP_ANONYMOUS
	#ifdef MAP_ANON
	#define MAP_ANONYMOUS MAP_ANON
	#endif
	#endif
	void *p = 0;
	if (m > n) {
		p = ::mmap(0, m, PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (p == MAP_FAILED) FLUX_SYSTEM_EXCEPTION;
	}

	p = ::mmap(p, n, PROT_READ | ((openFlags_ & (O_WRONLY|O_RDWR)) ? PROT_WRITE : 0), MAP_PRIVATE | ((p) ? MAP_FIXED : 0), fd_, i0);
	if (p == MAP_FAILED) FLUX_SYSTEM_EXCEPTION;

	#ifdef MADV_SEQUENTIAL
	if (::madvise(p, n, MADV_SEQUENTIAL) == -1)
		FLUX_SYSTEM_EXCEPTION;
	#endif

	Ref<ByteArray> data = new ByteArray((char*)p, n, m);
	if (nt > 0) data->truncate(nt, n);

	return data;
}

void File::sync()
{
	if (::fsync(fd_) == -1)
		FLUX_SYSTEM_EXCEPTION;
}

void File::dataSync()
{
#if _POSIX_SYNCHRONIZED_IO > 0
	if (::fdatasync(fd_) == -1)
		FLUX_SYSTEM_EXCEPTION;
#else
	sync();
#endif
}

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

bool File::rename(String path, String newPath)
{
	return ::rename(path, newPath) != -1;
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
	Ref<Random> random = Random::open();
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
				candidate->at(i) = r;
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


String File::lookup(String fileName, StringList *dirs, int accessFlags)
{
	Ref<StringList> h;
	if (!dirs) {
		h = Process::env("PATH")->split(':');
		dirs = h;
	}
	String path;
	for (int i = 0; i < dirs->size(); ++i) {
		String candidate = Format() << dirs->at(i) << "/" << fileName;
		if (access(candidate, accessFlags)) {
			path = candidate;
			break;
		}
	}
	return path;
}

Ref<FileStatus> File::status(String path)
{
	return FileStatus::read(path, true);
}

Ref<FileStatus> File::unresolvedStatus(String path)
{
	return FileStatus::read(path, false);
}

String File::load(String path)
{
	establish(path);
	return open(path)->readAll();
}

void File::save(String path, String text)
{
	establish(path);
	Ref<File> file = open(path, File::WriteOnly);
	file->truncate(0);
	file->write(text);
}

} // namespace flux
