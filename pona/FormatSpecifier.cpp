/*
 * FormatSpecifier.cpp -- syntax of "%%"-placeholders inside 'Format' strings
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "String.hpp"
#include "FormatSpecifier.hpp"

namespace pona
{

FormatSpecifier::FormatSpecifier()
{
	width_ = DEFINE("width", REPEAT(1, 2, RANGE('0', '9')));
	integerWidth_ = DEFINE("integerWidth", REPEAT(1, 2, RANGE('0', '9')));
	fractionWidth_ = DEFINE("fractionWidth", REPEAT(1, 2, RANGE('0', '9')));
	
	base_ =
		DEFINE("base",
			CHOICE(
				STRING("dec"),
				STRING("hex"),
				STRING("oct"),
				STRING("bin")
			)
		);
	
	exp_ = DEFINE("exp", RANGE("eE"));
	
	blank_ = DEFINE("blank",
		GLUE(
			CHAR('\''),
			RANGE(32, 127),
			CHAR('\'')
		)
	);
	
	format_ =
		DEFINE("format",
			GLUE(
				CHAR('%'),
				REPEAT(0, 5,
					GLUE(
						CHOICE(
							GLUE(
								REF("integerWidth"),
								CHAR('.'),
								REPEAT(0, 1,
									REF("fractionWidth")
								)
							),
							GLUE(
								CHAR('.'),
								REF("fractionWidth")
							),
							REF("width"),
							REF("base"),
							REF("exp"),
							REF("blank")
						),
						REPEAT(0, 1, CHAR(':'))
					)
				),
				CHAR('%')
			)
		);
	
	ENTRY("format");
	LINK();
}

bool FormatSpecifier::find(String text, int* i0, int* i1, int* w, int* wi, int* wf, int* base, bool* exp, Char* blank)
{
	Ref<Token, Owner> rootToken = 0;
	
	uint8_t buf[sizeof(Token) * 36 /* 5 times each path(7) + root(1) */];
	bool found =  Syntax<String::Media>::Definition::find(text, i0, i1, &rootToken, buf, sizeof(buf));
	
	if (found)
	{
		Ref<Token> token = rootToken->firstChild();
		
		while (token)
		{
			String value = text->copy(token->index(), token->length());
			
			if (token->rule() == width_->id())
			{
				*w = toInt(value);
			}
			else if (token->rule() == integerWidth_->id())
			{
				*wi = toInt(value);
			}
			else if (token->rule() == fractionWidth_->id())
			{
				*wf = toInt(value);
			}
			else if (token->rule() == base_->id())
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
			else if (token->rule() == exp_->id())
			{
				*exp = true;
			}
			else if (token->rule() == blank_->id())
			{
				*blank = value->get(1);
			}
			
			token = token->nextSibling();
		}
	}
	
	return found;
}

} // namespace pona
