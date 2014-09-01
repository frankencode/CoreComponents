/*
 * Copyright (C) 2013-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXCLAIM_HEADER_H
#define FLUXCLAIM_HEADER_H

#include <flux/Token.h>
#include <flux/String.h>

namespace fluxclaim {

using namespace flux;

class Header: public Object
{
public:
	inline static Ref<Header> create(String path, Token *token, String text, String message) {
		return new Header(path, token, text, message);
	}

	inline String path() const { return path_; }
	inline Token *token() const { return token_; }
	inline String text() const { return text_; }
	inline String message() const { return message_; }

private:
	Header(String path, Token *token, String text, String message)
		: path_(path),
		  token_(token),
		  text_(text),
		  message_(message)
	{}

	String path_;
	Ref<Token> token_;
	String text_;
	String message_;
};

} // namespace fluxclaim

#endif // FLUXCLAIM_HEADER_H
