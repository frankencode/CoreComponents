/*
 * IntegerLiteral.hpp -- syntax of an integer literal
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_INTEGERLITERAL_HPP
#define PONA_INTEGERLITERAL_HPP

#include "SyntaxDefinition.hpp"

namespace pona
{

class IntegerLiteral: public SyntaxDefinition<String::Media>
{
public:
	IntegerLiteral();
	bool match(String text, int i0, int* i1, uint64_t* value, int* sign);
	void read(String text, Ref<Token> rootToken, uint64_t* value, int* sign) const;
	
private:
	RULE sign_;
	RULE binNumber_;
	RULE octNumber_;
	RULE hexNumber_;
	RULE decNumber_;
};

} // namespace pona

#endif // PONA_INTEGERLITERAL_HPP

