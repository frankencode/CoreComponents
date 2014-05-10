/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXDOC_FRAGMENT_H
#define FLUXDOC_FRAGMENT_H

#include <flux/yason.h>

namespace fluxdoc
{

using namespace flux;

typedef YasonObject Fragment;
typedef List< Ref<Fragment> > FragmentList;

} // namespace fluxdoc

#endif // FLUXDOC_FRAGMENT_H
