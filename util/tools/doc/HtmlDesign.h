/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXDOC_HTMLDESIGN_H
#define FLUXDOC_HTMLDESIGN_H

#include <flux/toki/Theme.h>
#include "Design.h"

namespace flux {
namespace doc {

class HtmlDesign: public Design
{
public:
	static Ref<HtmlDesign> create() { return new HtmlDesign; }

	virtual void realize(const ByteArray *text, Token *objectToken);

	inline toki::Theme *tokiTheme() const { return tokiTheme_; }

private:
	HtmlDesign();
	virtual void define();
	virtual Ref<MetaObject> produce();

	Ref<toki::Theme> tokiTheme_;
};

}} // namespace flux::doc

#endif // FLUXDOC_HTMLDESIGN_H
