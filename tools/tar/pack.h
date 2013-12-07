/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/String.h>
#include <flux/ArchiveWriter.h>

namespace fluxtar
{

using namespace flux;

void pack(String path, ArchiveWriter *archive, bool verbose);

} // namespace fluxtar
