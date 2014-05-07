/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXDOC_MARKUP_H
#define FLUXDOC_MARKUP_H

#include <flux/SyntaxDefinition.h>
#include <flux/exceptions.h>
#include <flux/Singleton.h>
#include "Fragment.h"

namespace fluxdoc
{

using namespace flux;

typedef YasonObject Fragment;
typedef List< Ref<Fragment> > FragmentList;

class Markup: public SyntaxDefinition, public Singleton<Markup>
{
public:
	Ref<FragmentList> parse(ByteArray *text, String resource = "") const;

private:
	friend class Singleton<Markup>;
	Markup();
	Ref<FragmentList> readPart(ByteArray *text, Token *partToken) const;

	int escapedChar_;
	int object_;
	int line_;
	int chunk_;
	int heading_;
	int item_;
};

inline Markup *markup() { return Markup::instance(); }

} // namespace fluxdoc

#endif // FLUXDOC_MARKUP_H
