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

#ifndef PONA_INTEGERLITERAL_HPP
#define PONA_INTEGERLITERAL_HPP

#include "SyntaxDefinition.hpp"

namespace pona
{

class IntegerLiteral: public SyntaxDefinition<String>
{
public:
	IntegerLiteral();
	bool match(String text, int i0, int* i1, uint64_t* value, int* sign);
	
private:
	RULE sign_;
	RULE binNumber_;
	RULE octNumber_;
	RULE hexNumber_;
	RULE decNumber_;
};

} // namespace pona

#endif // PONA_INTEGERLITERAL_HPP

