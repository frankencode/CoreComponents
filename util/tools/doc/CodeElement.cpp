/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "CodeElement.h"

namespace flux {
namespace doc {

CodeElement::CodeElement(String className)
	: PathElement(className)
{}

void CodeElement::define()
{
	PathElement::define();
	insert("language", "");
}

void CodeElement::realize(const ByteArray *text, Token *objectToken)
{
	PathElement::realize(text, objectToken);
	language_ = value("language");
}

Ref<MetaObject> CodeElement::produce()
{
	return CodeElement::create();
}


}} // namespace flux::doc
