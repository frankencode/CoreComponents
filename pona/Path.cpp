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
#include "Format.hpp"
#include "Process.hpp"
#include "Path.hpp"

extern "C" char** environ;

namespace pona
{

Path::Path(UString path_)
	: path_(path_)
{}

bool Path::isRoot() const { return path_ == "/"; }

bool Path::isRelative() const { return !isAbsolute(); }
bool Path::isAbsolute() const {
	return (path_->length() > 0) ? (path_->get(0) == '/') : false;
}

Path Path::makeAbsoluteRelativeTo(UString currentDir) const
{
	if (isAbsolute())
		return path_;
	
	Ref<UStringList, Owner> absoluteParts = new UStringList;
	Ref<UStringList, Owner> parts = path_.split("/");
	
	int upCount = 0;
	
	for (int i = 0, n = parts->length(); i < n; ++i)
	{
		UString c = parts->get(i);
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
	
	UString absolutePath;
	if (currentDir->length() > 0)
		absolutePath = currentDir.deepCopy();
	else
		absolutePath = Process::cwd();
	
	while (upCount > 0) {
		absolutePath = Path(absolutePath).reduce();
		--upCount;
	}
	
	absoluteParts->insert(0, absolutePath);
	
	return UString(absoluteParts, "/");
}

Path Path::makeAbsolute() const
{
	if (isAbsolute())
		return path_;
	return makeAbsoluteRelativeTo(UString());
}

UString Path::fileName() const
{
	UString name;
	Ref<UStringList, Owner> parts = path_.split("/");
	if (parts->length() > 0)
		name = parts->get(-1);
	return name;
}

Path Path::reduce() const
{
	Ref<UStringList, Owner> parts = path_.split("/");
	if (parts->length() > 0)
		parts->pop(-1);
	UString resultPath = UString(parts, "/");
	if ((resultPath == "") && (isAbsolute()))
		resultPath = "/";
	return resultPath;
}

Path Path::expand(UString component) const
{
	return Path(Format() << path_ << "/" << component);
}

Path Path::lookup(Ref<UStringList> dirs, UString fileName, int accessFlags)
{
	UString path;
	for (int i = 0; i < dirs->length(); ++i) {
		UString candidate = Format() << dirs->get(i) << "/" << fileName;
		if (File(candidate).access(accessFlags)) {
			path = candidate;
			break;
		}
	}
	return path;
}

Path::operator UString() const { return path_; }

} // namespace pona
