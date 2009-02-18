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

#include "FormatSpecifier.hpp"

namespace pona
{

FormatSpecifier::FormatSpecifier()
{
	integerWidth_ =
		DEFINE("integerWidth",
			REPEAT(1, 2, RANGE('0', '9'))
		);
	
	fractionWidth_ =
		DEFINE("fractionWidth",
			REPEAT(1, 2, RANGE('0', '9'))
		);
	
	base_ =
		DEFINE("base",
			OR(
				OR(
					STRING("dec"),
					STRING("hex")
				),
				OR(
					STRING("oct"),
					STRING("bin")
				)
			)
		);
	
	DEFINE_SELF("format",
		CHAR('%',
			OR(
				REF("base",
					REPEAT(0, 1,
						CHAR(':',
							REF("integerWidth",
								REPEAT(0, 1,
									CHAR('.',
										REF("fractionWidth")
									)
								)
							)
						)
					)
				),
				REPEAT(0, 1,
					REF("integerWidth",
						REPEAT(0, 1,
							CHAR('.',
								REF("fractionWidth")
							)
						)
					)
				),
				CHAR('%')
			)
		)
	);
}

bool FormatSpecifier::find(String media, int* i0, int* i1, int* wi, int* wf, int* base)
{
	Ref<Token, Owner> rootToken = 0;
	
	uint8_t buf[sizeof(Token) * 6];
	bool found = SyntaxDefinition<String>::find(&media, i0, i1, &rootToken, buf, sizeof(buf));
	
	if (found)
	{
		Ref<Token> token = rootToken->firstChild();
		while (token)
		{
			String value = media.copy(token->index(), token->length());
			if (token->type() == base_->tokenType())
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
			else if (token->type() == integerWidth_->tokenType())
			{
				*wi = value.toInt();
			}
			else if (token->type() == fractionWidth_->tokenType())
			{
				*wf = value.toInt();
			}
			token = token->nextSibling();
		}
	}
	
	return found;
}

} // namespace pona
