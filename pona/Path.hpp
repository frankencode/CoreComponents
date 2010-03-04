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
#include "UString.hpp"
#include "File.hpp"

namespace pona
{

class Path
{
public:
	Path(UString path);
	
	bool isRoot() const;
	bool isRelative() const;
	bool isAbsolute() const;
	Path makeAbsoluteRelativeTo(UString currentDir) const;
	Path makeAbsolute() const;
	UString fileName() const;
	Path reduce() const;
	Path expand(UString component) const;
	
	static Path lookup(Ref<UStringList> dirs, UString fileName, int accessFlags = File::Exists);
	
	operator UString() const;
	
private:
	UString path_;
};

} // namespace pona

#endif // PONA_PATH_HPP
