/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "Singleton.h"
#include "IntegerSyntax.h"

namespace flux
{

IntegerSyntax::IntegerSyntax()
{
	SYNTAX("int");

	sign_ = DEFINE("Sign", RANGE("+-"));

	binNumber_ =
		DEFINE("BinNumber",
			GLUE(
				STRING("0b"),
				REPEAT(1, 256, RANGE('0', '1'))
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
				),
				NOT(RANGE(".eE"))
			)
		);

	ENTRY("Literal");
	LINK();
}

void IntegerSyntax::read(uint64_t *value, int *sign, const ByteArray *text, Token *token) const
{
	*sign = 1;
	*value = 0;

	token = token->firstChild();

	if (token->rule() == sign_)
	{
		if (text->at(token->index()) == '-')
			*sign = -1;
		token = token->nextSibling();
	}

	if (token->rule() == binNumber_)
	{
		for (int i = token->i0() + 2; i < token->i1(); ++i)
		{
			int x = text->at(i) - '0';
			*value *= 2;
			*value += x;
		}
	}
	else if (token->rule() == octNumber_)
	{
		for (int i = token->i0() + 1; i < token->i1(); ++i)
		{
			int x = text->at(i) - '0';
			*value *= 8;
			*value += x;
		}
	}
	else if (token->rule() == hexNumber_)
	{
		for (int i = token->i0() + 2; i < token->i1(); ++i)
		{
			int x = text->at(i);
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
			int x = text->at(i) - '0';
			*value *= 10;
			*value += x;
		}
	}
}

Ref<Token> IntegerSyntax::read(uint64_t *value, int *sign, const ByteArray *text, int i) const
{
	Ref<Token> token = match(text, i)->rootToken();
	if (token)
		read(value, sign, text, token);
	return token;
}

const IntegerSyntax *integerSyntax() { return Singleton<IntegerSyntax>::instance(); }

} // namespace flux
