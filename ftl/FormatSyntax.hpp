/*
 * FormatSyntax.hpp -- thread local syntax definitions (singletons)
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_FORMATSYNTAX_HPP
#define FTL_FORMATSYNTAX_HPP

#include "atoms"
#include "ThreadLocalSingleton.hpp"

namespace ftl
{

class FormatSpecifier;
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

} // namespace ftl

#endif // FTL_FORMATSYNTAX_HPP
