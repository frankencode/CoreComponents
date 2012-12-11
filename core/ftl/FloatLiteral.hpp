/*
 * FloatLiteral.hpp -- syntax of a floating point literal
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_FLOATLITERAL_HPP
#define FTL_FLOATLITERAL_HPP

#include "SyntaxDefinition.hpp"
#include "Singleton.hpp"

namespace ftl
{

class FloatLiteral: public SyntaxDefinition, public Singleton<FloatLiteral>
{
public:
	void read(float64_t *value, Ref<ByteArray> text, Ref<Token> token) const;
	Ref<Token, Owner> read(float64_t *value, Ref<ByteArray> text, int i = -1) const;

	inline int literal() const { return literal_; }

private:
	friend class Singleton<FloatLiteral>;

	FloatLiteral();

	int sign_;
	int integerPart_;
	int fractionPart_;
	int exponentSign_;
	int exponent_;
	int nan_;
	int infinite_;
	int literal_;
};

inline Ref<FloatLiteral> floatLiteral() { return FloatLiteral::instance(); }

} // namespace ftl

#endif // FTL_FLOATLITERAL_HPP
