/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXMAKE_RECIPEPROTOCOL_H
#define FLUXMAKE_RECIPEPROTOCOL_H

#include <flux/yason/yason.h>

namespace flux { template<class> class Singleton; }

namespace fluxmake
{

using namespace flux;
using namespace flux::yason;

class RecipeProtocol: public YasonProtocol
{
protected:
	friend class Singleton<RecipeProtocol>;
	RecipeProtocol();
};

const RecipeProtocol *recipeProtocol();

} // namespace fluxmake

#endif // FLUXMAKE_RECIPEPROTOCOL_H
