/*
 * IntegerLiteral.hpp -- syntax of an integer literal
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_INTEGERLITERAL_HPP
#define FTL_INTEGERLITERAL_HPP

#include "Syntax.hpp"

namespace ftl
{

class IntegerLiteral: public Syntax<ByteArray>::Definition
{
public:
	IntegerLiteral();
	bool match(Ref<ByteArray> text, int i0, int* i1, uint64_t* value, int* sign);
	void read(Ref<ByteArray> text, Ref<Token> rootToken, uint64_t* value, int* sign) const;
	
private:
	int sign_;
	int binNumber_;
	int octNumber_;
	int hexNumber_;
	int decNumber_;
	int integer_;
};

} // namespace ftl

#endif // FTL_INTEGERLITERAL_HPP
