/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "PatternSyntax.h"
#include "Pattern.h"

namespace flux
{

Pattern::Pattern() {}
Pattern::Pattern(const char *text) { *this = String(text); }
Pattern::Pattern(const String &text) { *this = text; }
Pattern::Pattern(const Ref<ByteArray> &text) { *this = String(text); }
Pattern::Pattern(const Variant &text) { *this = String(text); }

const Pattern &Pattern::operator=(const char *text)
{
	return *this = String(text);
}

const Pattern &Pattern::operator=(const String &text)
{
	if (text_ != text) {
		text_ = text;
		set(
			SyntaxDefinition::create(
				#ifndef NDEBUG
				SyntaxDebugger::create()
				#endif
			)
		);
		patternSyntax()->compile(text_, *this);
	}
	return *this;
}

const Pattern &Pattern::operator=(const Variant &text)
{
	return *this = String(text);
}

} // namespace flux
