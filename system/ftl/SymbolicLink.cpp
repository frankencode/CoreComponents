/*
 * SymbolicLink.cpp -- symbolic links
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include <unistd.h> // readlink
#include "Exception.hpp"
#include "Path.hpp"
#include "FileStatus.hpp"
#include "SymbolicLink.hpp"

namespace ftl
{

SymbolicLink::SymbolicLink(String path)
	: path_(path)
{}

String SymbolicLink::path() const { return path_; }

String SymbolicLink::read() const
{
	String buf = String::uninitialized(128);
	while (true) {
		ssize_t numBytes = ::readlink(path_, buf, buf->size());
		if (numBytes == -1)
			FTL_SYSTEM_EXCEPTION;
		if (numBytes <= buf->size()) {
			if (numBytes < buf->size())
				buf = String(buf->data(), numBytes);
			break;
		}
		buf = String::uninitialized(numBytes);
	}
	return buf;
}

String SymbolicLink::resolve() const
{
	Path path = path_;
	while (FileStatus(path, false).type() == File::SymbolicLink) {
		Path origPath = path;
		path = SymbolicLink(origPath.toString()).read();
		if (path.isRelative())
			path = origPath.reduce().expand(path);
	}
	return path;
}

} // namespace ftl
