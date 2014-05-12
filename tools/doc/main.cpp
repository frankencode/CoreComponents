/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/stdio.h>
#include <flux/Config.h>
#include "Document.h"
#include "Registry.h"

using namespace fluxdoc;

int main(int argc, char **argv)
{
	String toolName = String(argv[0])->fileName();
	try {
		Ref<Config> options = Config::read(argc, argv, YasonObject::create());
		for (int i = 0; i < options->arguments()->size(); ++i) {
			// String recipePath = options->arguments()->at(i) + "/Recipe";
			// File::open(recipePath);
			Ref<Document> document = Document::load(options->arguments()->at(i));
		}
	}
	catch (HelpError &) {
		fout(
			"Usage: %% [OPTION]... [DIR]\n"
			"Generate documentation from source DIR.\n"
			"\n"
		) << toolName;
		return 1;
	}
	catch (Exception &ex) {
		ferr() << toolName << ": " << ex.message() << nl;
		return 1;
	}

	return 0;
}
