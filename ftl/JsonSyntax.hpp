/*
 * JsonSyntax.hpp -- JSON syntax according to RFC 4627
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_JSONSYNTAX_HPP
#define FTL_JSONSYNTAX_HPP

#include "AbnfDefinition.hpp"

namespace ftl
{

class Json;

class JsonSyntax: public AbnfDefinition
{
public:
	JsonSyntax();
	
private:
	friend class Json;
	
	int sign_;
	int int_;
	int frac_;
	int exp_;
	int number_;
	int string_;
	int value_;
	int member_;
	int object_;
	int array_;
	int text_;
};

} // namespace ftl

#endif // FTL_JSONSYNTAX_HPP
