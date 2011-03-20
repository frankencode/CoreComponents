/*
 * Dir.hpp -- file directory I/O
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
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
	void create(int mode);
	void unlink();
	
	void open();
	void close();
	bool read(DirEntry* entry);
	bool isOpen() const;
	
private:
	String path_;
	DIR* dir_;
};

} // namespace ftl

#endif // FTL_DIR_HPP
