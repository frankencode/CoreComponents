/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
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
	inline SyntaxState *state() const { return state_; }

	virtual void realize(const ByteArray *text, Token *objectToken);

protected:
	CodeElement(String className = "Code");

	virtual void define();
	virtual Ref<MetaObject> produce();

	Ref<toki::Language> language_;
	Ref<SyntaxState> state_;
};

}} // namespace flux::doc

#endif // FLUXDOC_CODEELEMENT_H
