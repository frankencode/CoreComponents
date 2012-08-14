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

SymbolicLink::SymbolicLink(String path)
	: path_(path)
{}

String SymbolicLink::path() const { return path_; }

String SymbolicLink::read() const
{
	String buf = String(128);
	while (true) {
		ssize_t numBytes = ::readlink(path_, buf, buf->size());
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

String SymbolicLink::resolve() const
{
	String path = path_;
	while (FileStatus::newInstance(path, false)->type() == File::SymbolicLink) {
		String origPath = path;
		path = SymbolicLink(origPath).read();
		if (path->isRelativePath())
			path = origPath->reducePath()->expandPath(path);
	}
	return path;
}

} // namespace ftl
