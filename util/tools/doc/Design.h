/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXDOC_DESIGN_H
#define FLUXDOC_DESIGN_H

#include <flux/MetaObject.h>

namespace flux {
namespace doc {

class Design: public MetaObject
{
protected:
	Design(String className)
		: MetaObject(className)
	{
		insert("source", ""); // FIXME
	}
};

}} // namespace flux::doc

#endif // FLUXDOC_DESIGN_H
