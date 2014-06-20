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

using namespace flux;

class HtmlScreen: public TokenScreen, public Object
{
public:
	static Ref<HtmlScreen> create(String text, Stream *sink = 0) { return new HtmlScreen(text, sink); }

	virtual bool project(Token *token, int i0, int i1);

private:
	HtmlScreen(String text, Stream *sink);

	String text_;
	Format format_;
};

#endif // FLUXTOKI_HTMLSCREEN_H
