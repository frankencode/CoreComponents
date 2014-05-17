/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/stdio.h>
#include <flux/Arguments.h>
#include "Document.h"
#include "Design.h"
#include "Registry.h"
#include "Generator.h"

using namespace fluxdoc;

int main(int argc, char **argv)
{
	String toolName = String(argv[0])->fileName();
	try {
		Ref<Arguments> arguments = Arguments::parse(argc, argv);
		Ref<VariantMap> options = arguments->options();
		Ref<StringList> items = arguments->items();

		String defaultSourcePath = str(options->value("source"));
		Ref<Document> defaultDocument;
		if (defaultSourcePath != "") defaultDocument = Document::load(defaultSourcePath);

		for (int i = 0; i < items->size(); ++i) {
			String designPath = items->at(i);
			Ref<Design> design = yason::parse(File::open(designPath)->map(), registry()->designProtocol());
			arguments->validate(design);
			arguments->override(design);

			Ref<Document> document = defaultDocument;
			String sourcePath = design->value("source", "");
			if (defaultSourcePath != sourcePath && sourcePath != "") {
				document = Document::load(sourcePath);
			}
			if (!document) document = Document::parse("");

			Generator *generator = registry()->generatorByName(design->className());
			generator->run(design, document);
		}
	}
	catch (HelpError &) {
		fout(
			"Usage: %% [OPTION]... [FILE]...\n"
			"Produce documentation using different design FILEs.\n"
			"\n"
			"Options:\n"
			"  -source  source file\n"
		) << toolName;
		return 1;
	}
	catch (Exception &ex) {
		ferr() << toolName << ": " << ex.message() << nl;
		return 1;
	}

	return 0;
}
