/*
 * Path.hpp -- file paths
 *
 * Copyright (c) 2007-2009, Frank Mertens
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
	
	bool isRelative() const;
	bool isAbsolute() const;
	Path makeAbsolute() const;
	Path fileName() const;
	Path stripComponent() const;
	
	static Path lookup(Ref<StringList> dirs, String fileName, int accessFlags = File::Exists);
	
	operator String() const;
	
private:
	String path_;
};

} // namespace pona

#endif // PONA_PATH_HPP
