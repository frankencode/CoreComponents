/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXDOC_PATHELEMENT_H
#define FLUXDOC_PATHELEMENT_H

#include "TextElement.h"

namespace flux {
namespace doc {

class PathElement: public TextElement
{
public:
	inline String path() const { return path_; }
	inline String title() const { return title_; }

protected:
	PathElement(String className);

	virtual void define();
	virtual void realize(const ByteArray *text, Token *objectToken);

	String path_;
	String title_;
};

}} // namespace flux::doc

#endif // FLUXDOC_PATHELEMENT_H
