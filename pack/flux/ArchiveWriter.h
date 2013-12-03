/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_ARCHIVEWRITER_H
#define FLUX_ARCHIVEWRITER_H

#include <flux/String.h>

namespace flux
{

class ArchiveWriter: public Object
{
public:
	virtual void append(String path) = 0;
};

} // namespace flux

#endif // FLUX_ARCHIVEWRITER_H
