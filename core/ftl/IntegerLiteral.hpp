 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_INTEGERLITERAL_HPP
#define FTL_INTEGERLITERAL_HPP

#include "SyntaxDefinition.hpp"
#include "Singleton.hpp"

namespace ftl
{

class IntegerLiteral: public SyntaxDefinition, public Singleton<IntegerLiteral>
{
public:
	void read(uint64_t *value, int *sign, const ByteArray *text, Token *token) const;
	Ref<Token, Owner> read(uint64_t *value, int *sign, const ByteArray *text, int i = -1) const;

protected:
	friend class Singleton<IntegerLiteral>;

	IntegerLiteral();

	int sign_;
	int binNumber_;
	int octNumber_;
	int hexNumber_;
	int decNumber_;
	int literal_;
};

inline IntegerLiteral *integerLiteral() { return IntegerLiteral::instance(); }

} // namespace ftl

#endif // FTL_INTEGERLITERAL_HPP
