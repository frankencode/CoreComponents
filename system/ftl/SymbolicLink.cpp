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
#include "FileStatus.hpp"
#include "SymbolicLink.hpp"

namespace ftl
{

String SymbolicLink::read(String linkPath)
{
	String buf = String(128);
	while (true) {
		ssize_t numBytes = ::readlink(linkPath, buf, buf->size());
		if (numBytes == -1)
			FTL_SYSTEM_EXCEPTION;
		if (numBytes <= buf->size()) {
			if (numBytes < buf->size())
				buf = String(buf->data(), numBytes);
			break;
		}
		buf = String(numBytes);
	}
	return buf;
}

String SymbolicLink::resolve(String linkPath)
{
	String path = linkPath;
	while (FileStatus::newInstance(path, false)->type() == File::SymbolicLink) {
		String origPath = path;
		path = SymbolicLink::read(origPath);
		if (path->isRelativePath())
			path = origPath->reducePath()->expandPath(path);
	}
	return path;
}

void SymbolicLink::create(String origPath, String aliasPath)
{
	if (::symlink(origPath, aliasPath) == -1)
		FTL_SYSTEM_EXCEPTION;
}

} // namespace ftl
