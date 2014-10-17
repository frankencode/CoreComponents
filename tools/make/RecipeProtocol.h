/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXMAKE_RECIPEPROTOCOL_H
#define FLUXMAKE_RECIPEPROTOCOL_H

#include <flux/MetaProtocol>

namespace flux { template<class> class Singleton; }

namespace fluxmake {

using namespace flux;

class RecipeProtocol: public MetaProtocol
{
protected:
    friend class Singleton<RecipeProtocol>;
    RecipeProtocol();
};

const RecipeProtocol *recipeProtocol();

} // namespace fluxmake

#endif // FLUXMAKE_RECIPEPROTOCOL_H
