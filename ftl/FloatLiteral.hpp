/*
 * FloatLiteral.hpp -- syntax of a floating point literal
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_FLOATLITERAL_HPP
#define FTL_FLOATLITERAL_HPP

#include "Syntax.hpp"

namespace ftl
{

class FloatLiteral: public Syntax<ByteArray>::Definition
{
public:
	FloatLiteral();
	bool match(Ref<String::Media> text, int i0, int* i1, float64_t* value);
	float64_t read(Ref<String::Media> text, Ref<Token> token) const;
	
private:
	int sign_;
	int integerPart_;
	int fractionPart_;
	int exponentSign_;
	int exponent_;
	int nan_;
	int infinite_;
	int float_;
};

} // namespace ftl

#endif // FTL_FLOATLITERAL_HPP
