/*
 * DirEntry.hpp -- file entry in a directory file
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_DIRENTRY_HPP
#define FTL_DIRENTRY_HPP

#include <dirent.h>
#include "List.hpp"
#include "String.hpp"
#include "FileStatus.hpp"

namespace ftl
{

class Dir;

typedef struct dirent StructDirent;

class DirEntry: public StructDirent, public Instance
{
public:
	inline static Ref<DirEntry, Owner> newInstance() { return new DirEntry; }

	inline String path() const { return path_; }
	inline String name() const { return d_name; }
	#if (defined(BSD) || (defined(_BSD_SOURCE)))
	inline int type() const { return d_type; }
	#endif

	inline bool operator<(const DirEntry &b) const { return str::casecmp(d_name, b.d_name) < 0; }

protected:
	DirEntry() {}

	friend class Dir;
	String path_;
};

typedef List<DirEntry> DirEntryList;

} // namespace ftl

#endif // FTL_DIRENTRY_HPP
