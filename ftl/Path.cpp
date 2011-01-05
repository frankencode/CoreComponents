/*
 * Path.cpp -- file paths
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h> // kill
#include <unistd.h> // chdir, getcwd
#include <errno.h>
#include "Format.hpp"
#include "Process.hpp"
#include "Path.hpp"

namespace ftl
{

Path::Path(const char* path)
	: path_(path)
{}

Path::Path(const String& path)
	: path_(path)
{}

bool Path::isRoot() const { return path_ == "/"; }

bool Path::isRelative() const { return !isAbsolute(); }
bool Path::isAbsolute() const {
	return (path_->length() > 0) ? (path_->get(0) == '/') : false;
}

Path Path::absoluteRelativeTo(String currentDir) const
{
	if (isAbsolute())
		return path_;
	
	Ref<StringList, Owner> absoluteParts = new StringList;
	Ref<StringList, Owner> parts = path_.split("/");
	
	int upCount = 0;
	
	for (StringList::Index i = parts->first(); parts->def(i); ++i)
	{
		String c = parts->at(i);
		if (c == ".")
		{}
		else if (c == "..") {
			if (absoluteParts->length() > 0)
				absoluteParts->popBack();
			else
				++upCount;
		}
		else {
			absoluteParts->append(c);
		}
	}
	
	String absolutePath;
	if (currentDir->length() > 0)
		absolutePath = currentDir.copy();
	else
		absolutePath = Process::cwd();
	
	while (upCount > 0) {
		absolutePath = Path(absolutePath).reduce();
		--upCount;
	}
	
	absoluteParts->pushFront(absolutePath);
	
	return String::join(absoluteParts, "/");
}

Path Path::absolute() const
{
	if (isAbsolute())
		return path_;
	return absoluteRelativeTo(String());
}

String Path::fileName() const
{
	String name;
	Ref<StringList, Owner> parts = path_.split("/");
	if (parts->length() > 0)
		name = parts->at(-1);
	return name;
}

String Path::fileNameSansFirstExtension() const
{
	Ref<StringList, Owner> parts = fileName().split(".");
	parts->popBack();
	return String::join(parts, ".");
}

String Path::fileNameSansExtension() const
{
	Ref<StringList, Owner> parts = fileName().split(".");
	return parts->at(0);
}

Path Path::reduce() const
{
	Ref<StringList, Owner> parts = path_.split("/");
	if (parts->length() > 0)
		parts->popBack();
	String resultPath = String::join(parts, "/");
	if ((resultPath == "") && (isAbsolute()))
		resultPath = "/";
	return resultPath;
}

Path Path::expand(String component) const
{
	return Path(Format() << path_ << "/" << component);
}

Path Path::lookup(Ref<StringList> dirs, String fileName, int accessFlags)
{
	String path;
	for (StringList::Index i = dirs->first(); dirs->def(i); ++i) {
		String candidate = Format() << dirs->at(i) << "/" << fileName;
		if (File(candidate).access(accessFlags)) {
			path = candidate;
			break;
		}
	}
	return path;
}

} // namespace ftl
