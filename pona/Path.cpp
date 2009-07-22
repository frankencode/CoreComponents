/*
 * Path.cpp -- file paths
 *
 * Copyright (c) 2007-2009, Frank Mertens
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

bool Path::isRelative() const { return !isAbsolute(); }
bool Path::isAbsolute() const {
	return (path_->length() > 0) ? (path_->get(0) == '/') : false;
}

Path Path::makeAbsolute() const
{
	if (isAbsolute())
		return path_;
	
	Ref<StringList, Owner> absoluteParts = new StringList;
	Ref<StringList, Owner> parts = path_ / '/';
	
	for (int i = 0, n = parts->length(); i < n; ++i)
	{
		String c = parts->get(i);
		if (c == ".")
		{}
		else if (c == "..") {
			if (absoluteParts->length() > 0)
				absoluteParts->pop(-1);
		}
		else {
			absoluteParts->append(c);
		}
	}
	
	String absolutePath;
	if (!isAbsolute())
		absolutePath = Process::cwd();
	absolutePath << "/" << absoluteParts * '/';
	
	return absolutePath;
}

Path Path::fileName() const
{
	String name;
	Ref<StringList, Owner> parts = path_ / '/';
	if (parts->length() > 0)
		name = parts->get(-1);
	return name;
}

Path Path::stripComponent() const
{
	Ref<StringList, Owner> parts = path_ / '/';
	if (parts->length() > 0)
		parts->pop(-1);
	String resultPath = parts * '/';
	if ((resultPath == "") && (isAbsolute()))
		resultPath = "/";
	return resultPath;
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
