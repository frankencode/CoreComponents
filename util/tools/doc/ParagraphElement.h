/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
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
