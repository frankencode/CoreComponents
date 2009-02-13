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

#ifndef PONA_FORMATSPECIFIER_HPP
#define PONA_FORMATSPECIFIER_HPP

#include "SyntaxDefinition.hpp"

namespace pona
{

class FormatSpecifier: public SyntaxDefinition<String>
{
public:
	FormatSpecifier();
	bool find(String media, int* i0, int* i1, int* wi, int* wf, int* base);
	
private:
	RULE number_;
	RULE integerWidth_;
	RULE fractionWidth_;
	RULE base_;
};

} // namespace pona

#endif // PONA_FORMATSPECIFIER_HPP

