/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/stdio.h>
#include <flux/Config.h>
#include <flux/File.h>
#include <flux/InflateStream.h>

using namespace flux;

void unzip(Stream *stream);

int main(int argc, char **argv)
{
	try {
		Ref<YasonObject> prototype = YasonObject::create();
		Ref<Config> options = Config::read(argc, argv, prototype);
		for (int i = 0; i < options->arguments()->size(); ++i) {
			String path = options->arguments()->at(i);
			Ref<File> file = File::open(path);
			unzip(file);
		}
	}
	catch (HelpException &) {
		fout(
			"Usage: %% [FILE]...\n"
			"Unzip files compressed in gzip format.\n"
		) << String(argv[0])->fileName();
	}
	catch (UserException &ex) {
		ferr() << ex.message() << nl;
		return 1;
	}

	return 0;
}

void unzip(Stream *stream)
{
	Ref<InflateStream> source = InflateStream::open(stream);
	Ref<ByteArray> buf = ByteArray::create(0x4000);
	while (true) {
		int fill = source->read(buf);
		if (fill == 0) break;
		out()->write(ByteRange(buf, 0, fill));
	}
}
