/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXTOKI_HTMLSCREEN_H
#define FLUXTOKI_HTMLSCREEN_H

#include <flux/Token.h>
#include <flux/Format.h>
#include "Theme.h"

namespace flux {
namespace toki {

class Theme;

class HtmlScreen: public Object, public TokenScreen
{
public:
	static Ref<HtmlScreen> create(String text, Format sink = Format()) { return new HtmlScreen(text, sink); }

	virtual bool project(Token *token, int i0, int i1);

	static void writeCss(Theme *theme, Stream *sink = 0);

private:
	HtmlScreen(String text, Format sink);
	~HtmlScreen();

	void writeLineNumbers();

	String text_;
	Format sink_;
};

}} // namespace flux::toki

#endif // FLUXTOKI_HTMLSCREEN_H
