/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXDOC_MARKUPSYNTAX_H
#define FLUXDOC_MARKUPSYNTAX_H

#include <flux/SyntaxDefinition.h>
#include <flux/exceptions.h>
#include "Element.h"

namespace flux {

template<class> class Singleton;

namespace doc {

class MarkupSyntax: public SyntaxDefinition
{
public:
	Ref<ElementList> parse(ByteArray *text, String resource = "") const;

private:
	friend class Singleton<MarkupSyntax>;
	MarkupSyntax();
	Ref<ElementList> readPart(ByteArray *text, Token *partToken) const;
	Ref<Element> readList(ByteArray *text, Token *listToken) const;
	Ref<Element> readItem(ByteArray *text, Token *itemToken) const;
	String readLines(ByteArray *text, Token *lineToken) const;
	String readChunks(ByteArray *text, Token *chunkToken) const;


	int escapedChar_;
	int comment_;
	int object_;
	int chunk_;
	int line_;
	int paragraph_;
	int heading_;
	int item_;
	int list_;
};

const MarkupSyntax *markupSyntax();

}} // namespace flux::doc

#endif // FLUXDOC_MARKUPSYNTAX_H
