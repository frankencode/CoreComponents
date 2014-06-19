/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXTAR_ARWRITER_H
#define FLUXTAR_ARWRITER_H

#include <flux/Stream.h>
#include "ArchiveWriter.h"

namespace flux {
namespace tar {

class ArWriter: public ArchiveWriter
{
public:
	static Ref<ArWriter> open(Stream *sink);

	void writeFile(String path);

private:
	ArWriter(Stream *sink);

	Ref<Stream> sink_;
	bool firstTime_;
};

}} // namespace flux::tar

#endif // FLUXTAR_ARWRITER_H
