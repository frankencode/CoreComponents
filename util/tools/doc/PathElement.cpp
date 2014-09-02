/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
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
