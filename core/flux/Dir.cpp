/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <sys/stat.h> // mkdir
#include "File.h"
#include "FileStatus.h"
#include "Exception.h"
#include "Dir.h"

namespace flux
{

Ref<Dir> Dir::tryOpen(String path)
{
	DIR *dir = ::opendir(path);
	if (dir) return new Dir(path, dir);
	return 0;
}

Dir::Dir(String path, DIR *dir)
	: path_(path),
	  dir_(dir)
{
	if (!dir_) {
		dir_ = ::opendir(path_);
		if (!dir_)
			FLUX_SYSTEM_EXCEPTION;
	}
}

Dir::~Dir()
{
	if (::closedir(dir_) == -1)
		FLUX_SYSTEM_EXCEPTION;
}

String Dir::path() const { return path_; }

String Dir::path(String name) const
{
	if (path_ == ".") return name;
	if (path_ == "/") return "/" + name;
	return path_ + "/" + name;
}

bool Dir::read(String *name)
{
	struct dirent buf;
	struct dirent *result;
	memclr(&buf, sizeof(buf));
	int errorCode = ::readdir_r(dir_, &buf, &result);
	if (errorCode)
		throw SystemException(__FILE__, __LINE__, "SystemException", strcat("readdir_r() failed: error code = ", flux::intToStr(errorCode)), errorCode);
	if (result)
		*name = buf.d_name;
	return result;
}

bool Dir::access(String path, int flags)
{
	return access(path, flags);
}

bool Dir::exists(String path)
{
	return File::exists(path) && (File::status(path)->type() == File::Directory);
}

bool Dir::create(String path, int mode)
{
	return ::mkdir(path, mode) != -1;
}

bool Dir::establish(String path, int mode)
{
	Ref<StringList> missingDirs = StringList::create();
	while ((path != "") && (path != "/")) {
		if (Dir::exists(path)) break;
		missingDirs->pushFront(path);
		path = path->reducePath();
	}
	while (missingDirs->size() > 0) {
		if (!Dir::create(missingDirs->popFront(), mode))
			return false;
	}
	return true;
}

bool Dir::unlink(String path)
{
	return ::rmdir(path) != -1;
}

} // namespace flux
