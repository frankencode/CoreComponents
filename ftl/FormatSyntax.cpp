/*
 * FormatSyntax.cpp -- thread local syntax definitions (singletons)
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include "FormatSpecifier.hpp"
#include "IntegerLiteral.hpp"
#include "FloatLiteral.hpp"

#include "FormatSyntax.hpp"

namespace ftl
{

FormatSyntax::FormatSyntax()
{
	formatSpecifier_ = new FormatSpecifier;
	integerLiteral_ = new IntegerLiteral;
	floatingPointLiteral_ = new FloatLiteral;
}

Ref<FormatSpecifier> FormatSyntax::formatSpecifier() const { return formatSpecifier_; }
Ref<IntegerLiteral> FormatSyntax::integerLiteral() const { return integerLiteral_; }
Ref<FloatLiteral> FormatSyntax::floatingPointLiteral() const { return floatingPointLiteral_; }

} // namespace ftl
