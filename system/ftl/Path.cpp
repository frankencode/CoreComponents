/*
 * Path.cpp -- file paths
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
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
	: String(path)
{}

Path::Path(const String& path)
	: String(path)
{}

bool Path::isRoot() const { return String(*this) == "/"; }

bool Path::isRelative() const { return !isAbsolute(); }
bool Path::isAbsolute() const {
	return ((*this)->length() > 0) ? ((*this)->get(0) == '/') : false;
}

Path Path::absoluteRelativeTo(String currentDir) const
{
	if ((isAbsolute()) || (currentDir == "."))
		return *this;

	Ref<StringList, Owner> absoluteParts = new StringList;
	Ref<StringList, Owner> parts = (*this)->split("/");

	int upCount = 0;

	for (int i = 0; i < parts->length(); ++i)
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
		absolutePath = currentDir->copy();
	else
		absolutePath = Process::cwd();

	while (upCount > 0) {
		absolutePath = Path(absolutePath).reduce();
		--upCount;
	}

	absoluteParts->pushFront(absolutePath);

	return absoluteParts->join("/");
}

Path Path::absolute() const
{
	if (isAbsolute())
		return *this;
	return absoluteRelativeTo(String());
}

String Path::fileName() const
{
	String name;
	Ref<StringList, Owner> parts = (*this)->split("/");
	if (parts->length() > 0)
		name = parts->at(-1);
	return name;
}

String Path::fileNameSansFirstExtension() const
{
	Ref<StringList, Owner> parts = fileName()->split(".");
	parts->popBack();
	return parts->join(".");
}

String Path::fileNameSansExtension() const
{
	Ref<StringList, Owner> parts = fileName()->split(".");
	return parts->at(0);
}

Path Path::reduce() const
{
	Ref<StringList, Owner> parts = (*this)->split("/");
	if (parts->length() > 0)
		parts->popBack();
	String resultPath = parts->join("/");
	if ((resultPath == "") && (isAbsolute()))
		resultPath = "/";
	return resultPath;
}

Path Path::expand(String component) const
{
	return Path(Format() << *this << "/" << component);
}

Path Path::lookup(String fileName, Ref<StringList> dirs, int accessFlags)
{
	Ref<StringList, Owner> h;
	if (!dirs) {
		h = Process::env("PATH")->split(':');
		dirs = h;
	}
	String path;
	for (int i = 0; i < dirs->length(); ++i) {
		String candidate = Format() << dirs->at(i) << "/" << fileName;
		if (File(candidate).access(accessFlags)) {
			path = candidate;
			break;
		}
	}
	return path;
}

} // namespace ftl
