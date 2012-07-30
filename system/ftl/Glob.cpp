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
	dir_ = new Dir(path);
	dir_->open();
	pattern_ = remainder_->pop(0);
}

bool Glob::read(DirEntry* entry)
{
	if (child_) {
		if (child_->read(entry))
			return true;
		child_ = 0;
	}
	while (dir_->read(entry)) {
		if (pattern_->match(entry->name())) {
			if (remainder_->length() == 0)
				return true;
			String path = (StringList() << dir_->path() << "/" << entry->name()).join();
			if (Dir(path).exists()) {
				child_ = new Glob(path, remainder_);
				return read(entry);
			}
		}
	}
	return false;
}

} // namespace ftl
