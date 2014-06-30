/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXDOC_TITLETELEMENT_H
#define FLUXDOC_TITLETELEMENT_H

#include "TextElement.h"

namespace flux {
namespace doc {

class TitleElement: public TextElement
{
public:
	static Ref<TitleElement> create() { return new TitleElement; }

protected:
	TitleElement(String className = "Title")
		: TextElement(className)
	{}

	virtual Ref<MetaObject> produce()
	{
		return TitleElement::create();
	}
};

}} // namespace flux::doc

#endif // FLUXDOC_TITLETELEMENT_H
