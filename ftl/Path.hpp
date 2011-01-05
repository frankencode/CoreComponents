/*
 * Path.hpp -- file paths
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_PATH_HPP
#define FTL_PATH_HPP

#include <sys/types.h>
#include "atoms"
#include "String.hpp"
#include "File.hpp"

namespace ftl
{

class Path
{
public:
	Path(const char* path);
	Path(const String& path);
	
	bool isRoot() const;
	bool isRelative() const;
	bool isAbsolute() const;
	Path absoluteRelativeTo(String currentDir) const;
	Path absolute() const;
	String fileName() const;
	String fileNameSansFirstExtension() const;
	String fileNameSansExtension() const;
	
	Path reduce() const;
	Path expand(String component) const;
	
	static Path lookup(Ref<StringList> dirs, String fileName, int accessFlags = File::Exists);
	
	inline operator String() const { return path_; }
	inline String toString() const { return path_; }
	
private:
	String path_;
};

} // namespace ftl

#endif // FTL_PATH_HPP
