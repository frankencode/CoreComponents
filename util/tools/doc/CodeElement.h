/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXDOC_CODEELEMENT_H
#define FLUXDOC_CODEELEMENT_H

#include <flux/toki/Language.h>
#include "PathElement.h"

namespace flux {
namespace doc {

class CodeElement: public PathElement
{
public:
	static Ref<CodeElement> create() { return new CodeElement; }

	inline toki::Language *language() const { return language_; }

	virtual void realize(const ByteArray *text, Token *objectToken);

protected:
	CodeElement(String className = "Code");

	virtual void define();
	virtual Ref<MetaObject> produce();

	Ref<toki::Language> language_;
};

}} // namespace flux::doc

#endif // FLUXDOC_CODEELEMENT_H
