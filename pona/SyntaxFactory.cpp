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

#include "SyntaxFactory.hpp"

namespace pona
{

Ref<SyntaxFactory> SyntaxFactory::instance()
{
	static Ref<SyntaxFactory, ThreadLocalOwner> instance_ = 0;
	
	if (!instance_)
		instance_ = new SyntaxFactory;
	return instance_;
}

SyntaxFactory::SyntaxFactory()
{
	formatSpecifier_ = new FormatSpecifier;
	booleanLiteral_ = new BooleanLiteral;
	integerLiteral_ = new IntegerLiteral;
	floatingPointLiteral_ = new FloatingPointLiteral;
}

} // namespace pona
