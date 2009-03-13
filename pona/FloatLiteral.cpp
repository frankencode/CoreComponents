/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstraction Library.
**
** Copyright (C) 2007-2009  Frank Mertens
**
** This file is part of a free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License as published
** by the Free Software Foundation, either version 3 of the License,
** or (at your option) any later version.
**
** The library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this libary.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include <math.h>
#include "FloatLiteral.hpp"

namespace pona
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
	
	DEFINE_SELF("float",
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
}

bool FloatLiteral::match(String text, int i0, int* i1, float64_t* value)
{
	Ref<Token, Owner> rootToken;
	uint8_t buf[sizeof(Token) * 7];
	
	bool conform = SyntaxDefinition<String>::match(&text, i0, i1, &rootToken, 0, buf, sizeof(buf));
	
	if (conform)
		*value = read(text, rootToken);
	
	return conform;
}

float64_t FloatLiteral::read(String text, Ref<Token> rootToken) const
{
	float64_t value;
	
	Ref<Token> token = rootToken->firstChild();
	
	if (token->rule() == nan_->id())
	{
		value = nan;
	}
	else if (token->rule() == infinite_->id())
	{
		float64_t one, zero;
		one = 1.; zero = 0.;
		
		if (text.get(token->index()) == '-')
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
			if (token->rule() == sign_->id())
			{
				if (text.get(token->index()) == '-')
					sign = -1;
			}
			else if (token->rule() == integerPart_->id())
			{
				for (int i = token->i0(); i < token->i1(); ++i)
				{
					mantissa *= 10;
					mantissa += text.get(i) - '0';
				}
			}
			else if (token->rule() == fractionPart_->id())
			{
				float64_t h = 0.1;
				for (int i = token->i0(); i < token->i1(); ++i, h /= 10)
					mantissa += h * (text.get(i) - '0');
			}
			else if (token->rule() == exponentSign_->id())
			{
				if (text.get(token->index()) == '-')
					epSign = -1;
			}
			else if (token->rule() == exponent_->id())
			{
				for (int i = token->i0(); i < token->i1(); ++i)
				{
					ep *= 10;
					ep += text.get(i) - '0';
				}
			}
			
			token = token->nextSibling();
		}
		
		value = sign * mantissa * ::pow(10., float64_t(epSign * ep));
	}
	
	return value;
}

} // namespace pona
