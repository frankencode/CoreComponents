/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
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
