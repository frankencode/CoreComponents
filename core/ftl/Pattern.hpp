 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_PATTERN_HPP
#define FTL_PATTERN_HPP

#ifndef NDEBUG
#include "SyntaxDebugger.hpp"
#endif
#include "SyntaxDefinition.hpp"
#include "String.hpp"

namespace ftl
{

class PatternException: public StdException
{
public:
	PatternException(const String &error, int pos);
	~PatternException() throw();
	const char *what() const throw();

private:
	String message_;
	String error_;
	int pos_;
};

class Pattern: public hook<SyntaxDefinition>
{
public:
	Pattern();
	Pattern(const char *text);
	Pattern(const String &text);
	Pattern(const Variant &variant);

	const Pattern &operator=(const char *text);
	const Pattern &operator=(const String &text);
	const Pattern &operator=(const Variant &text);

	inline operator String() const { return text_; }

private:
	String text_;
};

} // namespace ftl

#endif // FTL_PATTERN_HPP
