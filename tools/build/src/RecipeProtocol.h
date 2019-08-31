/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/meta/MetaProtocol>

namespace cc { template<class> class Singleton; }

namespace ccbuild {

using namespace cc;
using namespace cc::meta;

class RecipeProtocol: public MetaProtocol
{
public:
    static const RecipeProtocol *instance();

protected:
    friend class Singleton<RecipeProtocol>;
    RecipeProtocol();
};

} // namespace ccbuild
