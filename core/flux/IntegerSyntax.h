/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_INTEGERSYNTAX_H
#define FLUX_INTEGERSYNTAX_H

#include "SyntaxDefinition.h"

namespace flux
{

class IntegerSyntax: public SyntaxDefinition
{
public:
	void read(uint64_t *value, int *sign, const ByteArray *text, Token *token) const;
	Ref<Token> read(uint64_t *value, int *sign, const ByteArray *text, int i = -1) const;

protected:
	friend class Singleton<IntegerSyntax>;

	IntegerSyntax();

	int sign_;
	int binNumber_;
	int octNumber_;
	int hexNumber_;
	int decNumber_;
	int literal_;
};

const IntegerSyntax *integerSyntax();

} // namespace flux

#endif // FLUX_INTEGERSYNTAX_H
