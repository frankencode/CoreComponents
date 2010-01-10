/*
 * Dir.hpp -- file directory I/O
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_DIR_HPP
#define PONA_DIR_HPP

#include "DirEntry.hpp"

namespace pona
{

class Dir: public Instance
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
	bool read(Ref<DirEntry> entry);
	
	bool isOpen() const;
	
private:
	String path_;
	DIR* dir_;
};

} // namespace pona

#endif // PONA_DIR_HPP
