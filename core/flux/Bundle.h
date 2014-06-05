/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_BUNDLE_H
#define FLUX_BUNDLE_H

#include "File.h"

namespace flux
{

#define FLUX_QUOTE(name) #name

#define FLUX_BUNDLE_LOOKUP(relPath) \
	File::lookup(relPath, StringList::create() << FLUX_QUOTE(FLUXMAKE_BUNDLE_PREFIX) << String(__FILE__)->reducePath(), File::Exists)

} // namespace flux

#endif // FLUX_BUNDLE_H
