/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXDOC_TEXTELEMENT_H
#define FLUXDOC_TEXTELEMENT_H

#include "Element.h"

namespace flux {
namespace doc {

class TextElement: public Element
{
public:
	inline String text() const { return text_; }

	virtual void realize(const ByteArray *, Token *)
	{
		text_ = value("text");
	}

protected:
	TextElement(String className, MetaProtocol *protocol = 0)
		: Element(className, protocol)
	{}

	virtual void define()
	{
		insert("text", "");
	}

	String text_;
};

}} // namespace flux::doc

#endif // FLUXDOC_TEXTELEMENT_H
