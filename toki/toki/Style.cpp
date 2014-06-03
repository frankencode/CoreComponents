/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "Style.h"

namespace fluxtoki
{

Ref<YasonObject> Style::produce()
{
	return Style::create();
}

void Style::define()
{
	className("Style");
	insert("name", "");
	insert("ink", Color());
	insert("paper", Color());
	insert("font", StringList::create());
	insert("bold", false);
	insert("italic", false);
}

void Style::realize(const ByteArray *text, Token *objectToken)
{
	name_ = value("name");
	ink_ = value("color");
	paper_ = value("paper");
	font_ = value("font");
	bold_ = value("bold");
	italic_ = value("italic");
}

} // namespace fluxtoki
