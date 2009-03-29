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
#include "IntegerLiteral.hpp"
#include "FloatLiteral.hpp"

#include "FormatSyntax.hpp"

namespace pona
{

Ref<FormatSyntax> FormatSyntax::instance()
{
	static Ref<FormatSyntax, ThreadLocalOwner> instance_ = 0;
	
	if (!instance_)
		instance_ = new FormatSyntax;
	return instance_;
}

FormatSyntax::FormatSyntax()
{
	formatSpecifier_ = new FormatSpecifier;
	integerLiteral_ = new IntegerLiteral;
	floatingPointLiteral_ = new FloatLiteral;
}

Ref<FormatSpecifier> FormatSyntax::formatSpecifier() const { return formatSpecifier_; }
Ref<IntegerLiteral> FormatSyntax::integerLiteral() const { return integerLiteral_; }
Ref<FloatLiteral> FormatSyntax::floatingPointLiteral() const { return floatingPointLiteral_; }

} // namespace pona
