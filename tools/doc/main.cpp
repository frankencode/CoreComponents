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
			String text = File::open(options->arguments()->at(i))->map();
			Ref<FragmentList> fragments = markup()->parse(text);
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
