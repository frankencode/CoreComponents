 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
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
	void read(float64_t *value, const ByteArray *text, Token *token) const;
	hook<Token> read(float64_t *value, const ByteArray *text, int i = -1) const;

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

inline FloatLiteral *floatLiteral() { return FloatLiteral::instance(); }

} // namespace ftl

#endif // FTL_FLOATLITERAL_HPP
