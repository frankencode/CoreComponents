/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/File.h>
#include "PathElement.h"

namespace flux {
namespace doc {

PathElement::PathElement(String className)
	: TextElement(className)
{}

void PathElement::define()
{
	TextElement::define();
	insert("path", "");
	insert("title", "");
}

void PathElement::realize(const ByteArray *text, Token *objectToken)
{
	TextElement::realize(text, objectToken);
	path_ = value("path");
	title_ = value("title");
	if (path_ != "") {
		try {
			text_ = File::open(path_)->map();
		}
		catch (SystemError &ex) {
			int offset = valueToken(text, objectToken, "path")->i1();
			throw SemanticError(ex.message(), text, offset);
		}
	}
}

}} // namespace flux::doc
