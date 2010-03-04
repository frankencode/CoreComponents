/*
 * FloatLiteral.hpp -- syntax of a floating point literal
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_FLOATLITERAL_HPP
#define PONA_FLOATLITERAL_HPP

#include "Syntax.hpp"

namespace pona
{

class FloatLiteral: public Syntax<UString::Media>::Definition
{
public:
	FloatLiteral();
	bool match(Ref<UString::Media> text, int i0, int* i1, float64_t* value);
	float64_t read(Ref<UString::Media> text, Ref<Token> token) const;
	
private:
	RULE sign_;
	RULE integerPart_;
	RULE fractionPart_;
	RULE exponentSign_;
	RULE exponent_;
	RULE nan_;
	RULE infinite_;
	RULE float_;
};

} // namespace pona

#endif // PONA_FLOATLITERAL_HPP
