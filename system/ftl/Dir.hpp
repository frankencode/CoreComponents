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

#include "DirEntry.hpp"

namespace ftl
{

class Dir: public Source<DirEntry>
{
public:
	Dir(String path);
	~Dir();

	String path() const;

	bool access(int flags);
	bool exists() const;
	void create(int mode = 0755);
	void unlink();

	void open();
	void close();
	bool read(DirEntry* entry);
	bool isOpen() const;

	static void establish(String path, int mode = 0755);

private:
	String path_;
	DIR* dir_;
};

} // namespace ftl

#endif // FTL_DIR_HPP
