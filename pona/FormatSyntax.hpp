/*
 * FormatSyntax.hpp -- thread local syntax definitions (singletons)
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_FORMATSYNTAX_HPP
#define PONA_FORMATSYNTAX_HPP

#include "atoms"
#include "ThreadLocalSingleton.hpp"

namespace pona
{

class FormatSpecifier;
class BooleanLiteral;
class IntegerLiteral;
class FloatLiteral;

class FormatSyntax: public Instance, public ThreadLocalSingleton<FormatSyntax>
{
public:
	Ref<FormatSpecifier> formatSpecifier() const;
	Ref<IntegerLiteral> integerLiteral() const;
	Ref<FloatLiteral> floatingPointLiteral() const;
	
private:
	friend class ThreadLocalSingleton<FormatSyntax>;
	
	FormatSyntax();
	
	Ref<FormatSpecifier, Owner> formatSpecifier_;
	Ref<IntegerLiteral, Owner> integerLiteral_;
	Ref<FloatLiteral, Owner> floatingPointLiteral_;
};

inline Ref<FormatSyntax> formatSyntax() { return FormatSyntax::instance(); }

} // namespace pona

#endif // PONA_FORMATSYNTAX_HPP
