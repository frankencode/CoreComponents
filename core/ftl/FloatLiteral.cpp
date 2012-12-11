/*
 * FloatLiteral.cpp -- syntax of a floating point literal
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include <math.h> // pow
#include "FloatLiteral.hpp"

namespace ftl
{

FloatLiteral::FloatLiteral()
{
	SYNTAX("float");

	sign_ = DEFINE("Sign", RANGE("+-"));
	integerPart_ = DEFINE("IntegerPart", REPEAT(1, 20, RANGE('0', '9')));
	fractionPart_ = DEFINE("FractionPart", REPEAT(1, 20, RANGE('0', '9')));
	exponentSign_ = DEFINE("ExponentSign", RANGE("+-"));
	exponent_ = DEFINE("Exponent",  REPEAT(1, 3, RANGE('0', '9')));
	nan_ = DEFINE("NaN", CHOICE(STRING("NaN"), STRING("nan")));
	infinite_ = DEFINE("Infinite", GLUE(REPEAT(0, 1, CHAR('-')), CHOICE(STRING("INFINITE"), STRING("inf"))));

	literal_ =
		DEFINE("Literal",
			CHOICE(
				REF("NaN"),
				REF("Infinite"),
				GLUE(
					REPEAT(0, 1, REF("Sign")),
					CHOICE(
						GLUE(
							REF("IntegerPart"),
							REPEAT(0, 1,
								GLUE(
									CHAR('.'),
									REPEAT(0, 1, REF("FractionPart"))
								)
							)
						),
						GLUE(
							CHAR('.'),
							REF("FractionPart")
						)
					),
					REPEAT(0, 1,
						GLUE(
							RANGE("eE"),
							REPEAT(0, 1, REF("ExponentSign")),
							REF("Exponent")
						)
					)
				)
			)
		);

	ENTRY("Literal");
	LINK();
}

void FloatLiteral::read(float64_t *value, Ref<ByteArray> text, Ref<Token> token) const
{
	token = token->firstChild();

	if (token->rule() == nan_)
	{
		*value = nan;
	}
	else if (token->rule() == infinite_)
	{
		float64_t one, zero;
		one = 1.; zero = 0.;

		if (text->get(token->index()) == '-')
			*value = -one / zero;
		else
			*value = one / zero;
	}
	else
	{
		int sign = 1;
		float64_t mantissa = 0.;
		int epSign = 0;
		int ep = 0;

		while (token)
		{
			if (token->rule() == sign_)
			{
				if (text->get(token->index()) == '-')
					sign = -1;
			}
			else if (token->rule() == integerPart_)
			{
				for (int i = token->i0(); i < token->i1(); ++i)
				{
					mantissa *= 10;
					mantissa += text->get(i) - '0';
				}
			}
			else if (token->rule() == fractionPart_)
			{
				float64_t h = 0.1;
				for (int i = token->i0(); i < token->i1(); ++i, h /= 10)
					mantissa += h * (text->get(i) - '0');
			}
			else if (token->rule() == exponentSign_)
			{
				if (text->get(token->index()) == '-')
					epSign = -1;
			}
			else if (token->rule() == exponent_)
			{
				for (int i = token->i0(); i < token->i1(); ++i)
				{
					ep *= 10;
					ep += text->get(i) - '0';
				}
			}

			token = token->nextSibling();
		}

		*value = sign * mantissa * ::pow(10., float64_t(epSign * ep));
	}
}

Ref<Token, Owner> FloatLiteral::read(float64_t *value, Ref<ByteArray> text, int i) const
{
	Ref<Token, Owner> token = match(text, i);
	if (token)
		read(value, text, token);
	return token;
}

} // namespace ftl
