/*
 * IntegerLiteral.hpp -- syntax of an integer literal
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_INTEGERLITERAL_HPP
#define FTL_INTEGERLITERAL_HPP

#include "Syntax.hpp"
#include "Singleton.hpp"

namespace ftl
{

class IntegerLiteral: public Syntax<ByteArray>::Definition, public Singleton<IntegerLiteral>
{
public:
	bool match(Ref<ByteArray> text, int i0, int* i1, uint64_t* value, int* sign);
	void read(Ref<ByteArray> text, Ref<Token> rootToken, uint64_t* value, int* sign) const;

private:
	friend class Singleton<IntegerLiteral>;
	IntegerLiteral();
	int sign_;
	int binNumber_;
	int octNumber_;
	int hexNumber_;
	int decNumber_;
	int literal_;
};

inline Ref<IntegerLiteral> integerLiteral() { return IntegerLiteral::instance(); }

} // namespace ftl

#endif // FTL_INTEGERLITERAL_HPP
