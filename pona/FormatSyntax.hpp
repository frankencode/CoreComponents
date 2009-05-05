/*
 * FormatSyntax.hpp -- thread local syntax definitions (singletons)
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_FORMATSYNTAX_HPP
#define PONA_FORMATSYNTAX_HPP

#include "atom"
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

