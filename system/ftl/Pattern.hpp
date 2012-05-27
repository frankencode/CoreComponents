/*
 * Pattern.hpp -- regular expressions
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_PATTERN_HPP
#define FTL_PATTERN_HPP

#include "Syntax.hpp"
#ifndef NDEBUG
#include "SyntaxDebugger.hpp"
#endif
#include "String.hpp"

namespace ftl
{

class PatternException: public StdException
{
public:
	PatternException(const String& error, int pos);
	~PatternException() throw();
	const char* what() const throw();
private:
	String message_;
	String error_;
	int pos_;
};

class Pattern: public Syntax<ByteArray>::Definition
{
public:
	Pattern(const char* text);
	Pattern(const String& text);
#ifndef NDEBUG
	Ref<Debugger> debugger() const { return debugFactory(); }
#endif
};

} // namespace ftl

#endif // FTL_PATTERN_HPP
