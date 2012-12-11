/*
 * Dir.hpp -- file directory I/O
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_DIR_HPP
#define FTL_DIR_HPP

#include <sys/types.h> // mode_t
#include <dirent.h> // DIR, opendir, closedir, readdir_r
#include "generics.hpp"
#include "String.hpp"

namespace ftl
{

class Dir: public Source<String>
{
public:
	inline static Ref<Dir, Owner> open(String path) { return new Dir(path); }
	static Ref<Dir, Owner> tryOpen(String path);
	~Dir();

	String path() const;
	String path(String name) const;
	bool read(String *name);

	static bool access(String path, int flags);
	static bool exists(String path);
	static bool create(String path, int mode = 0755);
	static bool establish(String path, int mode = 0755);
	static bool unlink(String path);

protected:
	Dir(String path, DIR *dir = 0);

	String path_;
	DIR *dir_;
};

} // namespace ftl

#endif // FTL_DIR_HPP
