/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_TEXTERROR_H
#define FLUX_TEXTERROR_H

#include "exceptions.h"
#include "SyntaxState.h"

namespace flux
{

class TextError: public Exception
{
public:
	inline String text() const { return text_; }
	inline int offset() const { return offset_; }

protected:
	TextError(String text, int offset)
		: text_(text),
		  offset_(offset)
	{}
	~TextError() throw() {}

	String text_;
	int offset_;
};

class SyntaxError: public TextError
{
public:
	SyntaxError(String text, SyntaxState *state = 0)
		: TextError(text, state ? state->hintOffset() : -1),
		  state_(state)
	{}
	~SyntaxError() throw() {}

	inline SyntaxState *state() const { return state_; }

	virtual String message() const;

private:
	SyntaxState *state_;
};

class SemanticError: public TextError
{
public:
	SemanticError(String reason, String text, int offset = -1)
		: TextError(text, offset),
		  reason_(reason)
	{}
	~SemanticError() throw() {}

	inline String reason() const { return reason_; }

	virtual String message() const;

private:
	String reason_;
};

} // namespace flux

#endif // FLUX_TEXTERROR_H
