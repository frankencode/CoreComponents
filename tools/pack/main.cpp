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
#include "unpack.h"

using namespace flux;

int main(int argc, char **argv)
{
	String toolName = String(argv[0])->fileName();
	bool packMode = !toolName->contains("unpack");
	try {
		Ref<YasonObject> prototype = YasonObject::create();
		prototype->insert("list", false);
		prototype->insert("status", false);
		prototype->insert("unpack", !packMode);
		prototype->insert("pack", packMode);
		Ref<Config> options = Config::read(argc, argv, prototype);
		bool listOption = options->value("list");
		bool statusOption = options->value("status");
		bool unpackOption = options->value("unpack");
		bool packOption = options->value("pack");
		if (options->arguments()->size() == 0) {
			if (listOption || statusOption || unpackOption) {
				Ref<ArchiveReader> archive = ArchiveReader::open(in());
				if (listOption) fluxpack::list(archive);
				else if (statusOption) fluxpack::status(archive);
				else if (unpackOption) fluxpack::unpack(archive);
			}
		}
		else {
			for (int i = 0; i < options->arguments()->size(); ++i) {
				String path = options->arguments()->at(i);
				if (listOption || statusOption || unpackOption) {
					Ref<ArchiveReader> archive = ArchiveReader::open(path);
					if (listOption) fluxpack::list(archive);
					else if (statusOption) fluxpack::status(archive);
					else if (unpackOption) fluxpack::unpack(archive);
				}
				else if (packOption) {
					// TODO
				}
			}
		}
	}
	catch (HelpException &) {
		if (packMode) {
			fout(
				"Usage: %% [DIR]...\n"
				"Pack archive files (see also: %%).\n"
			) << toolName << toolName->replace("pack", "unpack");
		}
		else {
			fout(
				"Usage: %% [OPTION]... [FILE]...\n"
				"Unpack archive files (see also: %%).\n"
				"\n"
				"Options:\n"
				"-list\tlist contents\n"
				"-status\tlist archived file status\n"
			) << toolName << toolName->replace("unpack", "pack");
		}
		return 1;
	}
	catch (UnsupportedArchiveFormat &ex) {
		ferr() << "Unsupported archive format" << nl;
		return 1;
	}
	catch (BrokenArchive &ex) {
		ferr() << "Broken archive: " << ex.reason() << " (file offset " << ex.offset() << ")" << nl;
		return 1;
	}
	catch (UserException &ex) {
		ferr() << ex.message() << nl;
		return 1;
	}

	return 0;
}
