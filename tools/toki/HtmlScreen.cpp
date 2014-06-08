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
	String s = text_->copy(i0, i1);
	if (s->contains('<')) s = s->replace("<", "&lt;");
	if (s->contains('>')) s = s->replace(">", "&gt;");
	bool whitespace = s->count(" \t\n\r") == s->size();
	if (!whitespace)
		format_ << "<span class=\"toki_" << hex(unsigned(token->scope())) << "_" << hex(token->rule()) << "\">";
	format_ << s;
	if (!whitespace)
		format_ << "</span>";
	if (format_->size() > 128) format_ << flush;
	return true;
}

} // namespace fluxtoki
