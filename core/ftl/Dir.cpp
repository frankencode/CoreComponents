 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */

#include <sys/stat.h> // mkdir

#include "File.hpp"
#include "FileStatus.hpp"
#include "Dir.hpp"

namespace ftl
{

Ref<Dir> Dir::tryOpen(string path)
{
	DIR *dir = ::opendir(path);
	if (dir) return new Dir(path, dir);
	return 0;
}

Dir::Dir(string path, DIR *dir)
	: path_(path),
	  dir_(dir)
{
	if (!dir_) {
		dir_ = ::opendir(path_);
		if (!dir_)
			FTL_SYSTEM_EXCEPTION;
	}
}

Dir::~Dir()
{
	if (::closedir(dir_) == -1)
		FTL_SYSTEM_EXCEPTION;
}

string Dir::path() const { return path_; }

string Dir::path(string name) const
{
	if (path_ == ".") return name;
	if (path_ == "/") return "/" + name;
	return path_ + "/" + name;
}

bool Dir::read(string *name)
{
	struct dirent buf;
	struct dirent *result;
	mem::clr(&buf, sizeof(buf));
	int errorCode = ::readdir_r(dir_, &buf, &result);
	if (errorCode)
		throw SystemException(__FILE__, __LINE__, "SystemException", str::cat("readdir_r() failed: error code = ", ftl::intToStr(errorCode)), errorCode);
	if (result)
		*name = buf.d_name;
	return result;
}

bool Dir::access(string path, int flags)
{
	return access(path, flags);
}

bool Dir::exists(string path)
{
	return File::exists(path) && (File::status(path)->type() == File::Directory);
}

bool Dir::create(string path, int mode)
{
	return ::mkdir(path, mode) != -1;
}

bool Dir::establish(string path, int mode)
{
	Ref<StringList> missingDirs = StringList::create();
	while ((path != "") && (path != "/")) {
		if (Dir::exists(path)) break;
		missingDirs->push(path);
		path = path->reducePath();
	}
	while (missingDirs->length() > 0) {
		if (!Dir::create(missingDirs->pop(), mode))
			return false;
	}
	return true;
}

bool Dir::unlink(string path)
{
	return ::rmdir(path) != -1;
}

} // namespace ftl
