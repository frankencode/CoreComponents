/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXDOC_DESIGN_H
#define FLUXDOC_DESIGN_H

#include <flux/MetaObject>

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
