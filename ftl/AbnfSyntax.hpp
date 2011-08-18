/*
 * AbnfSyntax.hpp -- ABNF compiler according to RFC5234
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#ifndef FTL_ABNFSYNTAX_HPP
#define FTL_ABNFSYNTAX_HPP

#include "AbnfCoreSyntax.hpp"

namespace ftl
{

class AbnfSyntax: public AbnfCoreSyntax
{
public:
	AbnfSyntax();
	
protected:
	NODE defineValue(const char* digitRule);
	
	int numVal_;
	int charVal_;
	int proseVal_;
	int group_;
	int option_;
	int element_;
	int repeat_;
	int repetition_;
	int concatenation_;
	int alternation_;
	int ruleName_;
	int definedAs_;
	int rule_;
	int rulelist_;
};

} // namespace ftl

#endif // FTL_ABNFSYNTAX_HPP
