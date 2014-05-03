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

class DebugToken: public Token
{
public:
	DebugToken(const char *ruleName)
		: ruleName_(ruleName)
	{}

	inline const char *ruleName() const { return ruleName_; }

private:
	const char *ruleName_;
};

} // namespace flux

#endif // FLUX_DEBUGTOKEN_H
