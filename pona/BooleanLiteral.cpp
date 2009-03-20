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

#include "BooleanLiteral.hpp"

namespace pona
{

BooleanLiteral::BooleanLiteral()
{
	true_ =
		DEFINE("true",
			CHOICE(
				GLUE(RANGE("tT"), STRING("rue")),
				GLUE(RANGE("oO"), CHAR('n')),
				STRING("TRUE"),
				STRING("ON")
			)
		);
	
	DEFINE("false",
		CHOICE(
			GLUE(RANGE("fF"), STRING("alse")),
			GLUE(RANGE("oO"), STRING("ff")),
			STRING("FALSE"),
			STRING("OFF")
		)
	);
	
	DEFINE_SELF(CHOICE(REF("true"), REF("false")));
}

bool BooleanLiteral::match(String text, int i0, int* i1, bool* value)
{
	Ref<Token, Owner> rootToken;
	uint8_t buf[sizeof(Token) * 3];
	
	bool conform = SyntaxDefinition<String::Media>::match(text, i0, i1, &rootToken, 0, buf, sizeof(buf));
	
	if (conform)
		*value = read(rootToken);
	
	return conform;
}

bool BooleanLiteral::read(Ref<Token> rootToken) const
{
	Ref<Token> token = rootToken->firstChild();
	return (token->rule() == true_->id());
}

} // namespace pona
