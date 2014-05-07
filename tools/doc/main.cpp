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
#include <flux/Yason.h> // DEBUG
#include "Markup.h"
#include "Registry.h"

using namespace fluxdoc;

int main(int argc, char **argv)
{
	String toolName = String(argv[0])->fileName();
	try {
		Ref<YasonObject> prototype = YasonObject::create();
		prototype->insert("format", "html");
		Ref<Config> options = Config::read(argc, argv, prototype);
		for (int i = 0; i < options->arguments()->size(); ++i) {
			String path = options->arguments()->at(i);
			String text = File::open(path)->map();
			Ref<FragmentList> fragments = markup()->parse(text, path);
			for (int i = 0; i < fragments->size(); ++i) ferr() << Yason::stringify(fragments->at(i));
		}
	}
	catch (HelpError &) {
		fout(
			"Usage: %% [OPTION]... [FILE]...\n"
			"Generate rich text from YML source files.\n"
			"\n"
			"Options:\n"
			"-format\toutput format (\"html\" by default)\n"
		) << toolName;
		return 1;
	}
	catch (Exception &ex) {
		ferr() << toolName << ": " << ex.what() << nl;
		return 1;
	}

	return 0;
}
