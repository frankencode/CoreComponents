/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
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
