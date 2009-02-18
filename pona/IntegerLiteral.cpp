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

#include "IntegerLiteral.hpp"

namespace pona
{

IntegerLiteral::IntegerLiteral()
{
	sign_ = DEFINE("sign", OR(CHAR('+'), CHAR('-')));
	
	binNumber_ =
		DEFINE("binNumber",
			REPEAT(1, 256,
				RANGE('0', '1'),
				CHAR('b')
			)
		);
		
	octNumber_ =
		DEFINE("octNumber",
			CHAR('0',
				REPEAT(1, 24,
					RANGE('0', '9')
				)
			)
		);
	
	hexNumber_ = 
		DEFINE("hexNumber",
			CHAR('0',
				CHAR('x',
					REPEAT(1, 20,
						OR(
							RANGE('0', '9'),
							OR(
								RANGE('a', 'f'),
								RANGE('A', 'F')
							)
						)
					)
				)
			)
		);
	
	decNumber_ =
		DEFINE("decNumber",
			REPEAT(1, 20,
				RANGE('0', '9')
			)
		);
	
	DEFINE_SELF("integer",
		REPEAT(0, 1,
			REF("sign"),
			OR(
				OR(
					REF("binNumber"),
					REF("octNumber")
				),
				OR(
					REF("hexNumber"),
					REF("decNumber")
				)
			)
		)
	);
}

bool IntegerLiteral::match(String text, int i0, int* i1, uint64_t* value, int* sign)
{
	Ref<Token, Owner> rootToken;
	uint8_t buf[sizeof(Token) * 6];
	
	bool conform = SyntaxDefinition<String>::match(&text, i0, i1, &rootToken, 0, buf, sizeof(buf));
	
	if (conform)
	{
		*sign = 1;
		*value = 0;
	
		Ref<Token> token = rootToken->firstChild();
		
		if (token->type() == sign_->tokenType())
		{
			if (text.get(token->index()) == '-')
				*sign = -1;
			token = token->nextSibling();
		}
		
		if (token->type() == binNumber_->tokenType())
		{
			for (int i = token->i0(); i < token->i1() - 1; ++i)
			{
				int x = text.get(i) - '0';
				*value *= 2;
				*value += x;
			}
		}
		else if (token->type() == octNumber_->tokenType())
		{
			for (int i = token->i0() + 1; i < token->i1(); ++i)
			{
				int x = text.get(i) - '0';
				*value *= 8;
				*value += x;
			}
		}
		else if (token->type() == hexNumber_->tokenType())
		{
			for (int i = token->i0() + 2; i < token->i1(); ++i)
			{
				int x = text.get(i);
				if (('0' <= x) && (x <= '9')) x -= '0';
				else if (('a' <= x) && (x <= 'z')) x -= 'a' - 10;
				else if (('A' <= x) && (x <= 'Z')) x -= 'A' - 10;
				*value *= 16;
				*value += x;
			}
		}
		else if (token->type() == decNumber_->tokenType())
		{
			for (int i = token->i0(); i < token->i1(); ++i)
			{
				int x = text.get(i) - '0';
				*value *= 10;
				*value += x;
			}
		}
	}
	
	return conform;
}

} // namespace pona
