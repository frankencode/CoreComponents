/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "RecipeProtocol.h"

namespace fluxdoc
{

class ProductionPrototype: public YasonObject
{
public:
	static Ref<ProductionPrototype> create() { return new ProductionPrototype; }

private:
	ProductionPrototype()
		: YasonObject("Production", YasonProtocol::create())
	{
		insert("source", "");
	}
};

RecipeProtocol::RecipeProtocol()
{
	productionProtocol_ = define<ProductionPrototype>()->protocol();
}

} // namespace fluxdoc
