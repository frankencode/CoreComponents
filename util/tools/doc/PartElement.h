/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_PARTELEMENT_H
#define FLUX_PARTELEMENT_H

#include "PathElement.h"

namespace flux {
namespace doc {

class PartElement: public PathElement
{
public:
	static Ref<PartElement> create() { return new PartElement; }

protected:
	PartElement(String className = "Part")
		: PathElement(className)
	{}

	virtual Ref<MetaObject> produce()
	{
		return create();
	}
};

}} // namespace flux::doc

#endif // FLUX_PARTELEMENT_H
