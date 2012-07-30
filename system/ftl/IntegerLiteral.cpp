/*
 * IntegerLiteral.cpp -- syntax of an integer literal
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include "IntegerLiteral.hpp"

namespace ftl
{

IntegerLiteral::IntegerLiteral()
{
	SYNTAX("int");

	sign_ = DEFINE("Sign", RANGE("+-"));

	binNumber_ =
		DEFINE("BinNumber",
			GLUE(
				REPEAT(1, 256, RANGE('0', '1')),
				CHAR('b')
			)
		);

	octNumber_ =
		DEFINE("OctNumber",
			GLUE(
				CHAR('0'),
				REPEAT(1, 24, RANGE('0', '9'))
			)
		);

	hexNumber_ =
		DEFINE("HexNumber",
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
		DEFINE("DecNumber",
			REPEAT(1, 20,
				RANGE('0', '9')
			)
		);

	literal_ =
		DEFINE("Literal",
			GLUE(
				REPEAT(0, 1, REF("Sign")),
				CHOICE(
					REF("BinNumber"),
					REF("OctNumber"),
					REF("HexNumber"),
					REF("DecNumber")
				)
			)
		);

	ENTRY("Literal");
	LINK();
}

void IntegerLiteral::read(uint64_t* value, int* sign, Ref<ByteArray> text, Ref<Token> token) const
{
	*sign = 1;
	*value = 0;

	token = token->firstChild();

	if (token->rule() == sign_)
	{
		if (text->get(token->index()) == '-')
			*sign = -1;
		token = token->nextSibling();
	}

	if (token->rule() == binNumber_)
	{
		for (int i = token->i0(); i < token->i1() - 1; ++i)
		{
			int x = text->get(i) - '0';
			*value *= 2;
			*value += x;
		}
	}
	else if (token->rule() == octNumber_)
	{
		for (int i = token->i0() + 1; i < token->i1(); ++i)
		{
			int x = text->get(i) - '0';
			*value *= 8;
			*value += x;
		}
	}
	else if (token->rule() == hexNumber_)
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
	else if (token->rule() == decNumber_)
	{
		for (int i = token->i0(); i < token->i1(); ++i)
		{
			int x = text->get(i) - '0';
			*value *= 10;
			*value += x;
		}
	}
}

Ref<Token, Owner> IntegerLiteral::read(uint64_t* value, int* sign, Ref<ByteArray> text, int i) const
{
	Ref<Token, Owner> token = match(text, i);
	if (token)
		read(value, sign, text, token);
	return token;
}

} // namespace ftl
