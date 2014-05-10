/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXDOC_RECIPEPROTOCOL_H
#define FLUXDOC_RECIPEPROTOCOL_H

#include <flux/Singleton.h>
#include <flux/yason.h>

namespace fluxdoc
{

using namespace flux;

class RecipeProtocol: public YasonProtocol, public Singleton<RecipeProtocol>
{
public:
	inline YasonProtocol *productionProtocol() const { return productionProtocol_; }

private:
	RecipeProtocol();
	friend class Singleton<RecipeProtocol>;

	Ref<YasonProtocol> productionProtocol_;
};

inline RecipeProtocol *recipeProtocol() { return RecipeProtocol::instance(); }

} // namespace fluxdoc

#endif // FLUXDOC_RECIPEPROTOCOL_H
