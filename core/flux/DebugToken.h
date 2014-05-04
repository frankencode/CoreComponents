/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_DEBUGTOKEN_H
#define FLUX_DEBUGTOKEN_H

#include "Token.h"

namespace flux
{

class DebugTokenFactory;
class Stream;

class DebugToken: public Token
{
	friend class DebugTokenFactory;

public:
	inline const char *scopeName() const { return scopeName_; }
	inline const char *ruleName() const { return ruleName_; }

	void printTo(Stream *stream, ByteArray *text, int depth = 0, int defaultScope = -1);

protected:
	DebugToken(int scope, int rule, const char *scopeName, const char *ruleName)
		: Token(scope, rule),
		  scopeName_(scopeName),
		  ruleName_(ruleName)
	{}

private:
	const char *scopeName_;
	const char *ruleName_;
};

} // namespace flux

#endif // FLUX_DEBUGTOKEN_H
