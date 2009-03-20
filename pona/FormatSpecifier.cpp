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

#include "NumberFormatting.hpp"
#include "FormatSpecifier.hpp"

namespace pona
{

FormatSpecifier::FormatSpecifier()
{
	integerWidth_ = DEFINE("integerWidth", REPEAT(1, 2, RANGE('0', '9')));
	fractionWidth_ = DEFINE("fractionWidth", REPEAT(1, 2, RANGE('0', '9')));
	
	base_ =
		DEFINE("base",
			GLUE(
				CHOICE(
					STRING("dec"),
					STRING("hex"),
					STRING("oct"),
					STRING("bin")
				),
				REPEAT(0, 1, CHAR(':'))
			)
		);
	
	DEFINE_SELF(
		GLUE(
			CHAR('%'),
			REPEAT(0, 1, REF("base")),
			REPEAT(0, 1, REF("integerWidth")),
			REPEAT(0, 1, GLUE(CHAR('.'), REF("fractionWidth"))),
			CHAR('%')
		)
	);
}

bool FormatSpecifier::find(String text, int* i0, int* i1, int* wi, int* wf, int* base)
{
	Ref<Token, Owner> rootToken = 0;
	
	uint8_t buf[sizeof(Token) * 6];
	bool found = SyntaxDefinition<String::Media>::find(text, i0, i1, &rootToken, buf, sizeof(buf));
	
	if (found)
	{
		Ref<Token> token = rootToken->firstChild();
		while (token)
		{
			String value = text->copy(token->index(), token->length());
			if (token->rule() == base_->id())
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
			else if (token->rule() == integerWidth_->id())
			{
				*wi = toInt(value);
			}
			else if (token->rule() == fractionWidth_->id())
			{
				*wf = toInt(value);
			}
			token = token->nextSibling();
		}
	}
	
	return found;
}

} // namespace pona
