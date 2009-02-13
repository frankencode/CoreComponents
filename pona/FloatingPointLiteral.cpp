/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstractions Library.
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
#include "LinePrinter.hpp"
#include "StdStream.hpp"
#include "FloatingPointLiteral.hpp"

namespace pona
{

FloatingPointLiteral::FloatingPointLiteral()
{
	sign_ = DEFINE("sign", OR(CHAR('+'), CHAR('-')));
	integerPart_ = DEFINE("integerPart", REPEAT(1, 20, RANGE('0', '9')));
	fractionPart_ = DEFINE("fractionPart", REPEAT(1, 20, RANGE('0', '9')));
	exponentSign_ = DEFINE("exponentSign", OR(CHAR('+'), CHAR('-')));
	exponent_ = DEFINE("exponent",  REPEAT(1, 3, RANGE('0', '9')));
	nan_ = DEFINE("nan", OR(STRING("NaN"), STRING("nan")));
	infinite_ = DEFINE("infinite", REPEAT(0, 1, CHAR('-'), OR(STRING("INFINITE"), STRING("inf"))));
	
	DEFINE_SELF("floatingPoint",
	  OR(
	    OR(
	      REF(nan_),
	      REF(infinite_)
	    ),
	    REPEAT(0, 1,
	      REF(sign_),
	      OR(
	        REF(integerPart_,
	          REPEAT(0, 1,
	            CHAR('.',
	              REPEAT(0, 1,
	                REF(fractionPart_)
	              )
	            )
	          )
	        ),
	        CHAR('.',
	          REF(fractionPart_)
	        ),
	        REPEAT(0, 1,
	          OR(
	            CHAR('e'),
	            CHAR('E'),
	            REPEAT(0, 1,
	              REF(exponentSign_),
	              REF(exponent_)
	            )
	          )
	        )
	      )
	    )
	  )
	);
}

bool FloatingPointLiteral::match(String text, int i0, int* i1, float64_t* value)
{
	Ref<Token, Owner> rootToken;
	uint8_t buf[sizeof(Token) * 7];
	
	bool conform = SyntaxDefinition<String>::match(&text, i0, i1, &rootToken, 0, buf, sizeof(buf));
	
	int sign = 1;
	float64_t mantissa = 0.;
	int epSign = 0;
	int ep = 0;
	
	if (conform)
	{
		Ref<Token> token = rootToken->firstChild();
		
		if (token->type() == nan_->tokenType())
		{
			*value = nan;
		}
		else if (token->type() == infinite_->tokenType())
		{
			float64_t one, zero;
			one = 1.; zero = 0.;
			
			if (text.get(token->index()) == '-')
				*value = -one / zero;
			else
				*value = one / zero;
		}
		else
		{
			while (token)
			{
				if (token->type() == sign_->tokenType())
				{
					if (text.get(token->index()) == '-')
						sign = -1;
				}
				else if (token->type() == integerPart_->tokenType())
				{
					for (int i = token->i0(); i < token->i1(); ++i)
					{
						mantissa *= 10;
						mantissa += text.get(i) - '0';
					}
				}
				else if (token->type() == fractionPart_->tokenType())
				{
					float64_t h = 0.1;
					for (int i = token->i0(); i < token->i1(); ++i, h /= 10)
						mantissa += h * (text.get(i) - '0');
				}
				else if (token->type() == exponentSign_->tokenType())
				{
					if (text.get(token->index()) == '-')
						epSign = -1;
				}
				else if (token->type() == exponent_->tokenType())
				{
					for (int i = token->i0(); i < token->i1(); ++i)
					{
						ep *= 10;
						ep += text.get(i) - '0';
					}
				}
				
				token = token->nextSibling();
			}
			
			*value = sign * mantissa * ::pow(10., float64_t(epSign * ep));
		}
	}
	
	return conform;
}

} // namespace pona
