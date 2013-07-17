/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FMAKE_RECIPEPROTOCOL_H
#define FMAKE_RECIPEPROTOCOL_H

#include <fkit/Yason.h>
#include <fkit/Singleton.h>

namespace fmake
{

using namespace fkit;

class RecipeProtocol: public YasonProtocol, public Singleton<RecipeProtocol>
{
protected:
	friend class Singleton<RecipeProtocol>;
	RecipeProtocol();

	template<class Prototype>
	void add() {
		Ref<Prototype> prototype = Prototype::create();
		insert(prototype->className(), prototype);
	}
};

inline RecipeProtocol *recipeProtocol() { return RecipeProtocol::instance(); }

} // namespace fmake

#endif // FMAKE_RECIPEPROTOCOL_H
