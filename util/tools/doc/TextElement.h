/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
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
