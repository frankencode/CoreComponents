/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/File.h>
#include <flux/ProcessFactory.h>
#include <flux/LookAheadStream.h>
#include "ArReader.h"
#include "TarReader.h"
#include "ArchiveReader.h"

namespace flux
{

Ref<ArchiveReader> ArchiveReader::open(String path)
{
	Ref<Stream> source;

	Ref<File> file = File::open(path);

	String suffix = path->suffix();
	String filter;
	if (suffix == "gz" || suffix == "tgz") filter = "gunzip";
	else if (suffix == "bz2") filter = "bunzip2";
	else if (suffix == "xz") filter = "unxz";

	if (filter != "") {
		Ref<ProcessFactory> factory = ProcessFactory::create();
		factory->setCommand(filter);
		factory->setIoPolicy(Process::ForwardOutput);
		factory->setIn(file);
		source = factory->produce();
	}
	else {
		source = file;
	}

	return ArchiveReader::open(source);
}

Ref<ArchiveReader> ArchiveReader::open(Stream *source)
{
	Ref<LookAheadStream> probe = LookAheadStream::open(source, 0x1000);
	if (ArReader::testFormat(probe)) {
		probe->done();
		return ArReader::open(probe);
	}
	probe->replay();
	if (TarReader::testFormat(probe)) {
		probe->done();
		return TarReader::open(probe);
	}
	throw UnsupportedArchiveFormat();
	return 0;
}

void ArchiveReader::skipData(ArchiveEntry *entry)
{
	readData(entry);
}

} // namespace flux
