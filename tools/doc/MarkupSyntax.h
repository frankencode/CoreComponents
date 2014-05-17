/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXDOC_MARKUPSYNTAX_H
#define FLUXDOC_MARKUPSYNTAX_H

#include <flux/SyntaxDefinition.h>
#include <flux/exceptions.h>
#include <flux/Singleton.h>
#include "Fragment.h"

namespace fluxdoc
{

using namespace flux;

class MarkupSyntax: public SyntaxDefinition, public Singleton<MarkupSyntax>
{
public:
	Ref<FragmentList> parse(ByteArray *text, String resource = "") const;

private:
	friend class Singleton<MarkupSyntax>;
	MarkupSyntax();
	Ref<FragmentList> readPart(ByteArray *text, Token *partToken) const;
	Ref<Fragment> readList(ByteArray *text, Token *listToken) const;
	Ref<Fragment> readItem(ByteArray *text, Token *itemToken) const;
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

inline MarkupSyntax *markupSyntax() { return MarkupSyntax::instance(); }

} // namespace fluxdoc

#endif // FLUXDOC_MARKUPSYNTAX_H
