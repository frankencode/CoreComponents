/*
 * Path.cpp -- file paths
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h> // kill
#include <unistd.h> // chdir, getcwd
#include <errno.h>
#include "Process.hpp"
#include "Path.hpp"

extern "C" char** environ;

namespace pona
{

Path::Path(String path_)
	: path_(path_)
{}

bool Path::isRoot() const { return path_ == "/"; }

bool Path::isRelative() const { return !isAbsolute(); }
bool Path::isAbsolute() const {
	return (path_->length() > 0) ? (path_->get(0) == '/') : false;
}

Path Path::makeAbsoluteRelativeTo(String currentDir) const
{
	if (isAbsolute())
		return path_;
	
	Ref<StringList, Owner> absoluteParts = new StringList;
	Ref<StringList, Owner> parts = path_ / '/';
	
	int upCount = 0;
	
	for (int i = 0, n = parts->length(); i < n; ++i)
	{
		String c = parts->get(i);
		if (c == ".")
		{}
		else if (c == "..") {
			if (absoluteParts->length() > 0)
				absoluteParts->pop(-1);
			else
				++upCount;
		}
		else {
			absoluteParts->append(c);
		}
	}
	
	String absolutePath;
	if (currentDir->length() > 0)
		absolutePath = currentDir->copy();
	else
		absolutePath = Process::cwd();
	
	while (upCount > 0) {
		absolutePath = Path(absolutePath).reduce();
		--upCount;
	}
	
	absolutePath << "/" << absoluteParts * '/';
	
	return absolutePath;
}

Path Path::makeAbsolute() const
{
	if (isAbsolute())
		return path_;
	return makeAbsoluteRelativeTo(String());
}

String Path::fileName() const
{
	String name;
	Ref<StringList, Owner> parts = path_ / '/';
	if (parts->length() > 0)
		name = parts->get(-1);
	return name;
}

Path Path::reduce() const
{
	Ref<StringList, Owner> parts = path_ / '/';
	if (parts->length() > 0)
		parts->pop(-1);
	String resultPath = parts * '/';
	if ((resultPath == "") && (isAbsolute()))
		resultPath = "/";
	return resultPath;
}

Path Path::expand(String component) const
{
	return Path(path_ + "/" + component);
}

Path Path::lookup(Ref<StringList> dirs, String fileName, int accessFlags)
{
	String path;
	for (int i = 0; i < dirs->length(); ++i) {
		String candidate = dirs->get(i) + "/" + fileName;
		if (File(candidate).access(accessFlags)) {
			path = candidate;
			break;
		}
	}
	return path;
}

Path::operator String() const { return path_; }

} // namespace pona
