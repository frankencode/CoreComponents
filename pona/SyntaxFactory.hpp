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

#ifndef PONA_SYNTAXFACTORY_HPP
#define PONA_SYNTAXFACTORY_HPP

#include "Atoms.hpp"
#include "FormatSpecifier.hpp"
#include "BooleanLiteral.hpp"
#include "IntegerLiteral.hpp"
#include "FloatingPointLiteral.hpp"

namespace pona
{

class SyntaxFactory: public Instance
{
public:
	static Ref<SyntaxFactory> instance();
	
	inline Ref<FormatSpecifier> formatSpecifier() { return formatSpecifier_; }
	
	inline Ref<BooleanLiteral> booleanLiteral() { return booleanLiteral_; }
	inline Ref<IntegerLiteral> integerLiteral() { return integerLiteral_; }
	inline Ref<FloatingPointLiteral> floatingPointLiteral() { return floatingPointLiteral_; }
	
private:
	SyntaxFactory();
	
	Ref<FormatSpecifier, Owner> formatSpecifier_;
	Ref<BooleanLiteral, Owner> booleanLiteral_;
	Ref<IntegerLiteral, Owner> integerLiteral_;
	Ref<FloatingPointLiteral, Owner> floatingPointLiteral_;
};

inline Ref<SyntaxFactory> syntaxFactory() { return SyntaxFactory::instance(); }

} // namespace pona

#endif // PONA_SYNTAXFACTORY_HPP

