/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/toki/Theme.h>
#include "HtmlDesign.h"

namespace flux {
namespace doc {

HtmlDesign::HtmlDesign()
	: Design("Html")
{}

void HtmlDesign::realize(const ByteArray *text, Token *objectToken)
{
	Design::realize(text, objectToken);
	String themePath = value("toki-theme");
	try {
		tokiTheme_ = toki::Theme::load(themePath);
	}
	catch (Exception &ex) {
		throw SemanticError(ex.message(), text, valueToken(text, objectToken, "toki-theme")->i1());
	}
}

void HtmlDesign::define()
{
	Design::define();
	insert("toki-theme", "ClassicWhite");
}

Ref<MetaObject> HtmlDesign::produce()
{
	return HtmlDesign::create();
}

}} // namespace flux::doc
