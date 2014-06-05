/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "HtmlScreen.h"

namespace fluxtoki
{

HtmlScreen::HtmlScreen(String text, Stream *sink)
	: text_(text),
	  format_(sink)
{}

bool HtmlScreen::project(Token *token, int i0, int i1)
{
	format_
		<< "<span class=\"toki_" << token->scope() << "_" << token->rule() << "\">"
		<< text_->copy(i0, i1)
		<< "</span>";
	return true;
}

} // namespace fluxtoki
