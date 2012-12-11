/*
 * FormatSpecifier.cpp -- syntax of "%%"-placeholders inside 'Format' strings
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include "String.hpp"
#include "FormatSpecifier.hpp"

namespace ftl
{

FormatSpecifier::FormatSpecifier()
{
	width_ = DEFINE("Width", REPEAT(1, 2, RANGE('0', '9')));
	integerWidth_ = DEFINE("IntegerWidth", REPEAT(1, 2, RANGE('0', '9')));
	fractionWidth_ = DEFINE("FractionWidth", REPEAT(1, 2, RANGE('0', '9')));

	base_ =
		DEFINE("Base",
			CHOICE(
				STRING("dec"),
				STRING("hex"),
				STRING("oct"),
				STRING("bin")
			)
		);

	exp_ = DEFINE("Exp", RANGE("eE"));

	blank_ = DEFINE("Blank",
		GLUE(
			CHAR('\''),
			RANGE(32, 127),
			CHAR('\'')
		)
	);

	format_ =
		DEFINE("Format",
			GLUE(
				CHAR('%'),
				REPEAT(0, 5,
					GLUE(
						CHOICE(
							GLUE(
								REF("IntegerWidth"),
								CHAR('.'),
								REPEAT(0, 1,
									REF("FractionWidth")
								)
							),
							GLUE(
								CHAR('.'),
								REF("FractionWidth")
							),
							REF("Width"),
							REF("Base"),
							REF("Exp"),
							REF("Blank")
						),
						REPEAT(0, 1, CHAR(':'))
					)
				),
				CHAR('%')
			)
		);

	ENTRY("Format");
	LINK();
}

bool FormatSpecifier::find(Ref<ByteArray> text, int *i0, int *i1, int *w, int *wi, int *wf, int *base, bool *exp, char *blank)
{
	Ref<Token, Owner> rootToken = SyntaxDefinition::find(text, *i0);

	if (rootToken)
	{
		if (i0) *i0 = rootToken->i0();
		if (i1) *i1 = rootToken->i1();

		Ref<Token> token = rootToken->firstChild();

		while (token)
		{
			String value = text->copy(token->i0(), token->i1());

			if (token->rule() == width_)
			{
				*w = value->toInt();
			}
			else if (token->rule() == integerWidth_)
			{
				*wi = value->toInt();
			}
			else if (token->rule() == fractionWidth_)
			{
				*wf = value->toInt();
			}
			else if (token->rule() == base_)
			{
				if (value == "dec")
					*base = 10;
				else if (value == "hex")
					*base = 16;
				else if (value == "oct")
					*base = 8;
				else if (value == "bin")
					*base = 2;
			}
			else if (token->rule() == exp_)
			{
				*exp = true;
			}
			else if (token->rule() == blank_)
			{
				*blank = value->get(1);
			}

			token = token->nextSibling();
		}
	}

	return rootToken;
}

} // namespace ftl
