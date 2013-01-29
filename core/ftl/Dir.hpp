 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_DIR_HPP
#define FTL_DIR_HPP

#include <sys/types.h> // mode_t
#include <dirent.h> // DIR, opendir, closedir, readdir_r
#include "generics.hpp"
#include "string.hpp"

namespace ftl
{

class Dir: public Source<string>
{
public:
	inline static hook<Dir> open(string path) { return new Dir(path); }
	static hook<Dir> tryOpen(string path);

	string path() const;
	string path(string name) const;
	bool read(string *name);

	static bool access(string path, int flags);
	static bool exists(string path);
	static bool create(string path, int mode = 0755);
	static bool establish(string path, int mode = 0755);
	static bool unlink(string path);

protected:
	Dir(string path, DIR *dir = 0);
	~Dir();

	string path_;
	DIR *dir_;
};

} // namespace ftl

#endif // FTL_DIR_HPP
