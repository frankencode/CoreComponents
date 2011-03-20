/*
 * Dir.cpp -- file directory I/O
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include <sys/stat.h> // mkdir
#include <sys/types.h> // mode_t

#include "Path.hpp"
#include "File.hpp"
#include "FileStatus.hpp"
#include "Dir.hpp"

namespace ftl
{

Dir::Dir(String path)
	: path_(Path(path).absolute()),
	  dir_(0)
{}

Dir::~Dir()
{
	if (isOpen()) close();
}

String Dir::path() const { return path_; }

bool Dir::access(int flags) { return File(path_).access(flags); }

bool Dir::exists() const
{
	return File(path_).exists() && (FileStatus(path_).type() == File::Directory);
}

void Dir::create(int mode)
{
	if (::mkdir(path_, mode) == -1)
		FTL_SYSTEM_EXCEPTION;
}

void Dir::unlink()
{
	if (::rmdir(path_))
		FTL_SYSTEM_EXCEPTION;
}

void Dir::open()
{
	if (dir_) return;
	dir_ = ::opendir(path_);
	if (!dir_)
		FTL_SYSTEM_EXCEPTION;
}

void Dir::close()
{
	if (!dir_) return;
	if (::closedir(dir_) == -1)
		FTL_SYSTEM_EXCEPTION;
	dir_ = 0;
}

bool Dir::read(DirEntry* entry)
{
	if (!isOpen()) open();
	struct dirent* buf = entry;
	struct dirent* result;
	mem::clr(buf, sizeof(struct dirent)); // for paranoid reason
	int errorCode = ::readdir_r(dir_, buf, &result);
	if (errorCode)
		throw SystemException(__FILE__, __LINE__, "SystemException", str::cat("readdir_r() failed: error code = ", ftl::intToStr(errorCode)), errorCode);
	
	entry->path_ = Path(entry->d_name).absoluteRelativeTo(path_);
	return result;
}

bool Dir::isOpen() const { return dir_; }

} // namespace ftl
