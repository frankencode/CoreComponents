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

#ifndef PONA_FLOATSYNTAX_HPP
#define PONA_FLOATSYNTAX_HPP

#include "SyntaxRule.hpp"

class FloatSyntax: public SyntaxRule<Char>
{
public:
	enum TokenType
	{
		signToken,
		numToken,
		floatToken
	};
	
	FloatSyntax()
	{
		sign_ = DEFINE("sign", signToken, OR(CHAR('+'), CHAR('-')));
		num_ = DEFINE("num", numToken, REPEAT(1, intMax, RANGE('0', '9')))
		DEFINE_SELF("float", floatToken,
			REPEAT(0, 1,
				REF(sign_),
				OR(
					REPEAT(0, 1, REF(num_), CHAR('.', REF(num_))),
					REF(num_, CHAR('.', REPEAT(0, 1, REF(num_)))),
					REPEAT(0, 1,
						OR(CHAR('e'), CHAR('E'),
							REPEAT(0, 1, REF(sign_),
								REF(num_)
							)
						)
					)
				)
			);
	}
	
	float64_t eval(String s)
	{
		// match(
		return 0;
	}
	
private:
	RULE sign_;
	RULE num_;
};

#endif // PONA_FLOATSYNTAX_HPP

