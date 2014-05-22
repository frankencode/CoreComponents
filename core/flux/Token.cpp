/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "Memory.h"
#include "Token.h"

namespace flux
{

bool Token::glow(TokenScreen *screen)
{
	int i = i0_;

	Token *child = firstChild();
	while (child) {
		if (i < child->i0_) {
			if (!screen->project(this, i, child->i0_))
				return false;
			i = child->i0_;
		}
		if (!child->glow(screen))
			return false;
		i = child->i1_;
		child = child->nextSibling();
	}

	if (i < i1_)
		if (!screen->project(this, i, i1_))
			return false;

	return true;
}

Token *Token::at(int i) const
{
	Token *token = const_cast<Token *>(this);
	Token *child = firstChild();
	while (child) {
		if ((child->i0() <= i) && (i < child->i1())) {
			token = child->at(i);
			break;
		}
		child = child->nextSibling();
	}
	return token;
}

} // namespace flux
