/*
 * JsonSyntax.hpp -- JSON syntax according to RFC 4627
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_JSONSYNTAX_HPP
#define FTL_JSONSYNTAX_HPP

#include "Syntax.hpp"

namespace ftl
{

class JsonSyntax: public Syntax<ByteArray>::Definition
{
public:
	JsonSyntax();
	
protected:
	int value_;
	int object_;
	int member_;
	int array_;
	int sign_;
	int int_;
	int frac_;
	int exp_;
	int number_;
};

} // namespace ftl

#endif // FTL_JSONSYNTAX_HPP
