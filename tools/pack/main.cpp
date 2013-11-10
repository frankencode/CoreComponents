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
#include <flux/Archive.h>

using namespace flux;

int main(int argc, char **argv)
{
	try {
		Ref<YasonObject> prototype = YasonObject::create();
		prototype->insert("list", false);
		prototype->insert("status", false);
		prototype->insert("bundle", false);
		prototype->insert("explode", false);
		Ref<Config> options = Config::read(argc, argv, prototype);
		if (options->value("list")) {
			for (int i = 0; i < options->arguments()->size(); ++i) {
				String path = options->arguments()->at(i);
				Ref<Archive> archive = Archive::open(path);
				for (Ref<ArchiveEntry> entry; archive->read(&entry);) {
					fout() << entry->path() << nl;
				}
			}
		}
	}
	catch (HelpException &) {
		fout(
			"Usage: %% [OPTION]... [FILE]...\n"
			"Pack, unpack or list the contents of an archive file.\n"
			"\n"
			"Options:\n"
			"  -list     list contents of an archive file\n"
			"  -status   show archived file status of each archive member\n"
			"  -bundle   pack files together to an archive file\n"
			"  -explode  unpack an archive file\n"
		) << String(argv[0])->fileName();
		return 1;
	}
	catch (UserException &ex) {
		fout() << ex.message() << nl;
		return 1;
	}

	return 0;
}
