/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_PATTERN_H
#define FLUX_PATTERN_H

#include "String.h"
#include "UserError.h"
#ifndef NDEBUG
#include "SyntaxDebugger.h"
#endif
#include "SyntaxDefinition.h"

namespace flux
{

class PatternException: public UserError
{
public:
	PatternException(const String &error, int pos);
};

class Pattern: public Ref<SyntaxDefinition>
{
public:
	Pattern();
	Pattern(const char *text);
	Pattern(const String &text);
	Pattern(const Variant &Variant);

	const Pattern &operator=(const char *text);
	const Pattern &operator=(const String &text);
	const Pattern &operator=(const Variant &text);

	inline operator String() const { return text_; }

private:
	friend String str(const Pattern &pattern);
	String text_;
};

inline String str(const Pattern &pattern) { return pattern.text_; }

} // namespace flux

#endif // FLUX_PATTERN_H
