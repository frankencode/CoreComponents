 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
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
		if (name == ".") continue;
		if ((name == "..") && (pattern_.text() != "..")) continue;
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
