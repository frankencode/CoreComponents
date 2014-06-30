/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXDOC_PARAGRAPHELEMENT_H
#define FLUXDOC_PARAGRAPHELEMENT_H

#include "TextElement.h"

namespace flux {
namespace doc {

class ParagraphElement: public TextElement
{
public:
	static Ref<ParagraphElement> create() { return new ParagraphElement; }

protected:
	ParagraphElement(String className = "Paragraph")
		: TextElement(className)
	{}

	virtual Ref<MetaObject> produce()
	{
		return create();
	}
};

}} // namespace flux::doc

#endif // FLUXDOC_PARAGRAPHELEMENT_H
