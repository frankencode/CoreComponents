/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_DIR_H
#define FLUX_DIR_H

#include <sys/types.h> // mode_t
#include <dirent.h> // DIR, opendir, closedir, readdir_r
#include "generics.h"
#include "String.h"

namespace flux
{

class Dir: public Source<String>
{
public:
	inline static Ref<Dir> open(String path) { return new Dir(path); }
	static Ref<Dir> tryOpen(String path);

	String path() const;
	String path(String name) const;
	bool read(String *name);

	static bool access(String path, int flags);
	static bool exists(String path);
	static void create(String path, int mode = 0755);
	static void establish(String path, int mode = 0755);
	static void unlink(String path);

protected:
	Dir(String path, DIR *dir = 0);
	~Dir();

	String path_;
	DIR *dir_;
};

} // namespace flux

#endif // FLUX_DIR_H
