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

#ifndef PONA_FLOATLITERAL_HPP
#define PONA_FLOATLITERAL_HPP

#include "SyntaxDefinition.hpp"

namespace pona
{

class FloatLiteral: public SyntaxDefinition<String>
{
public:
	FloatLiteral();
	bool match(String text, int i0, int* i1, float64_t* value);
	float64_t read(String text, Ref<Token> token) const;
	
private:
	RULE sign_;
	RULE integerPart_;
	RULE fractionPart_;
	RULE exponentSign_;
	RULE exponent_;
	RULE nan_;
	RULE infinite_;
};

} // namespace pona

#endif // PONA_FLOATLITERAL_HPP

