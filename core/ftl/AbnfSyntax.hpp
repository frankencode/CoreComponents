/*
 * AbnfSyntax.hpp -- ABNF compiler according to RFC5234
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_ABNFSYNTAX_HPP
#define FTL_ABNFSYNTAX_HPP

#include "AbnfCoreSyntax.hpp"

namespace ftl
{

class AbnfSyntax: public AbnfCoreSyntax
{
protected:
	AbnfSyntax();
	NODE defineValue(const char *digitRule);

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
