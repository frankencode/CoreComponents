/*
 * DirEntry.hpp -- file entry in a directory file
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_DIRENTRY_HPP
#define PONA_DIRENTRY_HPP

#include <dirent.h>
#include "List.hpp"
#include "String.hpp"
#include "FileStatus.hpp"

namespace pona
{

class Dir;

typedef struct dirent StructDirent;

class DirEntry: public StructDirent, public Instance
{
public:
	inline String path() const { return path_; }
	inline String name() const { return d_name; }
	#if (defined(BSD) || (defined(_BSD_SOURCE)))
	inline int type() const { return d_type; }
	#endif
	
	inline bool operator<(const DirEntry& b) const { return str::casecmp(d_name, b.d_name) < 0; }
	
private:
	friend class Dir;
	String path_;
};

typedef List<DirEntry> DirEntryList;

} // namespace pona

#endif // PONA_DIRENTRY_HPP
