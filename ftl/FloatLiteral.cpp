/*
 * FloatLiteral.cpp -- syntax of a floating point literal
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include <math.h> // pow
#include "FloatLiteral.hpp"

namespace ftl
{

FloatLiteral::FloatLiteral()
{
	sign_ = DEFINE("sign", RANGE("+-"));
	integerPart_ = DEFINE("integerPart", REPEAT(1, 20, RANGE('0', '9')));
	fractionPart_ = DEFINE("fractionPart", REPEAT(1, 20, RANGE('0', '9')));
	exponentSign_ = DEFINE("exponentSign", RANGE("+-"));
	exponent_ = DEFINE("exponent",  REPEAT(1, 3, RANGE('0', '9')));
	nan_ = DEFINE("nan", CHOICE(STRING("NaN"), STRING("nan")));
	infinite_ = DEFINE("infinite", GLUE(REPEAT(0, 1, CHAR('-')), CHOICE(STRING("INFINITE"), STRING("inf"))));
	
	float_ =
		DEFINE("float",
			CHOICE(
				REF("nan"),
				REF("infinite"),
				GLUE(
					REPEAT(0, 1, REF("sign")),
					CHOICE(
						GLUE(
							REF("integerPart"),
							REPEAT(0, 1,
								GLUE(
									CHAR('.'),
									REPEAT(0, 1, REF("fractionPart"))
								)
							)
						),
						GLUE(
							CHAR('.'),
							REF("fractionPart")
						)
					),
					REPEAT(0, 1,
						GLUE(
							RANGE("eE"),
							REPEAT(0, 1, REF("exponentSign")),
							REF("exponent")
						)
					)
				)
			)
		);
	
	ENTRY("float");
	LINK();
}

bool FloatLiteral::match(Ref<ByteArray> text, int i0, int* i1, float64_t* value)
{
	Ref<Token, Owner> rootToken = Syntax<ByteArray>::Definition::match(text, i0, i1);
	if (rootToken) *value = read(text, rootToken);
	return rootToken;
}

float64_t FloatLiteral::read(Ref<ByteArray> text, Ref<Token> rootToken) const
{
	float64_t value;
	
	Ref<Token> token = rootToken->firstChild();
	
	if (token->rule() == nan_)
	{
		value = nan;
	}
	else if (token->rule() == infinite_)
	{
		float64_t one, zero;
		one = 1.; zero = 0.;
		
		if (text->get(token->index()) == '-')
			value = -one / zero;
		else
			value = one / zero;
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
		
		value = sign * mantissa * ::pow(10., float64_t(epSign * ep));
	}
	
	return value;
}

} // namespace ftl
