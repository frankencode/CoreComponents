/*
 * FormatSyntax.cpp -- thread local syntax definitions (singletons)
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

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
