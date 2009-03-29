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

#ifndef PONA_FORMATSYNTAX_HPP
#define PONA_FORMATSYNTAX_HPP

#include "atoms"
#include "ThreadLocalOwner.hpp"

namespace pona
{

class FormatSpecifier;
class BooleanLiteral;
class IntegerLiteral;
class FloatLiteral;

class FormatSyntax: public Instance
{
public:
	static Ref<FormatSyntax> instance();
	
	Ref<FormatSpecifier> formatSpecifier() const;
	Ref<IntegerLiteral> integerLiteral() const;
	Ref<FloatLiteral> floatingPointLiteral() const;
	
private:
	FormatSyntax();
	
	Ref<FormatSpecifier, Owner> formatSpecifier_;
	Ref<IntegerLiteral, Owner> integerLiteral_;
	Ref<FloatLiteral, Owner> floatingPointLiteral_;
};

inline Ref<FormatSyntax> syntaxFactory() { return FormatSyntax::instance(); }

} // namespace pona

#endif // PONA_FORMATSYNTAX_HPP

