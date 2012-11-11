/*
 * Glob.cpp -- recursive directory listings
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include "Dir.hpp"
#include "Pattern.hpp"
#include "Glob.hpp"

namespace ftl
{

Glob::Glob(String expression)
	: remainder_(expression->split('/'))
{
	if (expression->head(1) == "/") {
		remainder_->pop(0);
		init("/");
	}
	else {
		init(".");
	}
}

Glob::Glob(String path, Ref<StringList> remainder)
	: remainder_(remainder->clone())
{
	init(path);
}

void Glob::init(String path)
{
	dir_ = Dir::open(path);
	pattern_ = remainder_->pop(0);
}

bool Glob::read(String *path)
{
	if (child_) {
		if (child_->read(path))
			return true;
		child_ = 0;
	}
	String name;
	while (dir_->read(&name)) {
		if ((name == ".") || (name == ".."))
			continue;
		if (pattern_->match(name)) {
			String node = dir_->path(name);
			if (remainder_->length() == 0) {
				*path = node;
				return true;
			}
			if (Dir::exists(node)) {
				child_ = new Glob(node, remainder_);
				return read(path);
			}
		}
	}
	return false;
}

} // namespace ftl
