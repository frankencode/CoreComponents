/*
 * IntegerLiteral.hpp -- syntax of an integer literal
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_INTEGERLITERAL_HPP
#define PONA_INTEGERLITERAL_HPP

#include "Syntax.hpp"

namespace pona
{

class IntegerLiteral: public Syntax<String::Media>::Definition
{
public:
	IntegerLiteral();
	bool match(Ref<String::Media> text, int i0, int* i1, uint64_t* value, int* sign);
	void read(Ref<String::Media> text, Ref<Token> rootToken, uint64_t* value, int* sign) const;
	
private:
	RULE sign_;
	RULE binNumber_;
	RULE octNumber_;
	RULE hexNumber_;
	RULE decNumber_;
	RULE integer_;
};

} // namespace pona

#endif // PONA_INTEGERLITERAL_HPP
