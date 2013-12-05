/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_ARWRITER_H
#define FLUX_ARWRITER_H

#include <flux/Stream.h>
#include "ArchiveWriter.h"

namespace flux
{

class ArWriter: public ArchiveWriter
{
public:
	static Ref<ArWriter> open(Stream *sink);

	void writeFile(String path);

private:
	ArWriter(Stream *sink);

	Ref<Stream> sink_;
};

} // namespace flux

#endif // FLUX_ARWRITER_H
