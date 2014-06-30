/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "PathElement.h"

namespace flux {
namespace doc {

class ImageElement: public PathElement
{
public:
	static Ref<ImageElement> create() { return new ImageElement; }

protected:
	ImageElement(String className = "Image")
		: PathElement(className)
	{}

	virtual Ref<MetaObject> produce()
	{
		return create();
	}
};

}} // namespace flux::doc
