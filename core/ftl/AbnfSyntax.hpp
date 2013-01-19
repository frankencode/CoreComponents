 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
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
