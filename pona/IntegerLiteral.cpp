/*
 * IntegerLiteral.cpp -- syntax of an integer literal
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "IntegerLiteral.hpp"

namespace pona
{

IntegerLiteral::IntegerLiteral()
{
	sign_ = DEFINE("sign", RANGE("+-"));
	
	binNumber_ =
		DEFINE("binNumber",
			GLUE(
				REPEAT(1, 256, RANGE('0', '1')),
				CHAR('b')
			)
		);
		
	octNumber_ =
		DEFINE("octNumber",
			GLUE(
				CHAR('0'),
				REPEAT(1, 24, RANGE('0', '9'))
			)
		);
	
	hexNumber_ = 
		DEFINE("hexNumber",
			GLUE(
				STRING("0x"),
				REPEAT(1, 20,
					CHOICE(
						RANGE('0', '9'),
						RANGE('a', 'f'),
						RANGE('A', 'F')
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
	
	integer_ =
		DEFINE("integer",
			GLUE(
				REPEAT(0, 1, REF("sign")),
				CHOICE(
					REF("binNumber"),
					REF("octNumber"),
					REF("hexNumber"),
					REF("decNumber")
				)
			)
		);
	
	ENTRY("integer");
	LINK();
}

bool IntegerLiteral::match(Ref<String::Media> text, int i0, int* i1, uint64_t* value, int* sign)
{
	Ref<Token, Owner> rootToken;
	uint8_t buf[sizeof(Token) * 6];
	
	bool conform = Syntax<String::Media>::Definition::match(text, i0, i1, &rootToken, 0, buf, sizeof(buf));
	
	if (conform)
		read(text, rootToken, value, sign);
	
	return conform;
}

void IntegerLiteral::read(Ref<String::Media> text, Ref<Token> rootToken, uint64_t* value, int* sign) const
{
	*sign = 1;
	*value = 0;

	Ref<Token> token = rootToken->firstChild();
	
	if (token->rule() == sign_->id())
	{
		if (text->get(token->index()) == '-')
			*sign = -1;
		token = token->nextSibling();
	}
	
	if (token->rule() == binNumber_->id())
	{
		for (int i = token->i0(); i < token->i1() - 1; ++i)
		{
			int x = text->get(i) - '0';
			*value *= 2;
			*value += x;
		}
	}
	else if (token->rule() == octNumber_->id())
	{
		for (int i = token->i0() + 1; i < token->i1(); ++i)
		{
			int x = text->get(i) - '0';
			*value *= 8;
			*value += x;
		}
	}
	else if (token->rule() == hexNumber_->id())
	{
		for (int i = token->i0() + 2; i < token->i1(); ++i)
		{
			int x = text->get(i);
			if (('0' <= x) && (x <= '9')) x -= '0';
			else if (('a' <= x) && (x <= 'z')) x -= 'a' - 10;
			else if (('A' <= x) && (x <= 'Z')) x -= 'A' - 10;
			*value *= 16;
			*value += x;
		}
	}
	else if (token->rule() == decNumber_->id())
	{
		for (int i = token->i0(); i < token->i1(); ++i)
		{
			int x = text->get(i) - '0';
			*value *= 10;
			*value += x;
		}
	}
}

} // namespace pona
