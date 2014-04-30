/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "Format.h"
#include "TextError.h"

namespace flux
{

String SyntaxError::message() const
{
	Format format;
	const char *error = "Syntax error";
	if (state_->hint()) {
		int line = 0, pos = 0;
		text_->offsetToLinePos(state_->hintOffset(), &line, &pos);
		format << line << ":" << pos << ": ";
	}
	format << error;
	if (state_->hint()) format << ": " << state_->hint();
	return format;
}

String SemanticError::message() const
{
	Format format;
	if (offset_ >= 0) {
		int line = 0, pos = 0;
		text_->offsetToLinePos(offset_, &line, &pos);
		format << line << ":" << pos << ": ";
	}
	format << reason_;
	return format;
}

} // namespace flux
