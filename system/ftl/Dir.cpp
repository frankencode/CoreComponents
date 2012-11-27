/*
 * Dir.cpp -- file directory I/O
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include <sys/stat.h> // mkdir

#include "File.hpp"
#include "FileStatus.hpp"
#include "Dir.hpp"

namespace ftl
{

Dir::Dir(String path)
	: path_(path),
	  dir_(::opendir(path_))
{
	if (!dir_)
		FTL_SYSTEM_EXCEPTION;
}

Dir::~Dir()
{
	if (::closedir(dir_) == -1)
		FTL_SYSTEM_EXCEPTION;
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
	mem::clr(&buf, sizeof(buf));
	int errorCode = ::readdir_r(dir_, &buf, &result);
	if (errorCode)
		throw SystemException(__FILE__, __LINE__, "SystemException", str::cat("readdir_r() failed: error code = ", ftl::intToStr(errorCode)), errorCode);
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
	Ref<StringList, Owner> missingDirs = StringList::create();
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

bool Dir::unlink(String path)
{
	return ::rmdir(path) != -1;
}

} // namespace ftl
