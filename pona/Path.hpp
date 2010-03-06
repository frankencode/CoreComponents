/*
 * Path.hpp -- file paths
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_PATH_HPP
#define PONA_PATH_HPP

#include <sys/types.h>
#include "atoms"
#include "String.hpp"
#include "File.hpp"

namespace pona
{

class Path
{
public:
	Path(String path);
	
	bool isRoot() const;
	bool isRelative() const;
	bool isAbsolute() const;
	Path makeAbsoluteRelativeTo(String currentDir) const;
	Path makeAbsolute() const;
	String fileName() const;
	Path reduce() const;
	Path expand(String component) const;
	
	static Path lookup(Ref<UStringList> dirs, String fileName, int accessFlags = File::Exists);
	
	operator String() const;
	
private:
	String path_;
};

} // namespace pona

#endif // PONA_PATH_HPP
