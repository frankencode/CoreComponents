/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXDOC_ELEMENT_H
#define FLUXDOC_ELEMENT_H

#include <flux/yason.h>

namespace fluxdoc
{

using namespace flux;

typedef YasonObject Element;
typedef List< Ref<Element> > ElementList;

} // namespace fluxdoc

#endif // FLUXDOC_ELEMENT_H
