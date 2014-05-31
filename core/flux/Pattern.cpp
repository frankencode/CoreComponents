/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "PatternSyntax.h"
#include "Pattern.h"

namespace flux
{

Pattern::Pattern()
{}

Pattern::Pattern(const char *text)
{
	*this = String(text);
}

Pattern::Pattern(const String &text)
{
	*this = text;
}

Pattern::Pattern(const Variant &text)
{
	*this = String(text);
}

const Pattern &Pattern::operator=(const char *text)
{
	return *this = String(text);
}

const Pattern &Pattern::operator=(const String &text)
{
	text_ = text;
	set(
		SyntaxDefinition::create(
#ifndef NDEBUG
			SyntaxDebugger::create()
#endif
		)
	);
	patternSyntax()->compile(text_, *this);
	return *this;
}

const Pattern &Pattern::operator=(const Variant &text)
{
	return *this = String(text);
}

} // namespace flux
