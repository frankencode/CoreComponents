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
#include <flux/File.h>
#include <toki/Theme.h>
#include "HtmlScreen.h"

namespace fluxtoki
{

using namespace flux;

void writeCss(Theme *theme)
{
	String cssPath = theme->name() + ".css";
	fout() << cssPath << nl; // verbose
	Ref<File> file = File::open(cssPath, File::Create|File::Truncate|File::WriteOnly);
	Format format(file);
	for (int i = 0; i < theme->paletteCount(); ++i) {
		const Palette *palette = theme->paletteAt(i);
		for (int j = 0; j < palette->styleCount(); ++j) {
			const Style *style = palette->styleAt(j);
			format << ".toki_" << hex(unsigned(palette->scope())) << "_" << hex(unsigned(style->rule())) << " { ";
			if (style->ink() != Color()) format << "color: " << style->ink() << "; ";
			if (style->paper() != Color()) format << "background-color: " << style->paper() << "; ";
			if (style->bold()) format << "font-weight: bold; ";
			if (style->italic()) format << "font-style: italic; ";
			format << "} /* " << palette->scopeName() << "::" << style->ruleName() << " */";
			format << nl;
		}
	}
}

void writeTags(String text, Token *token, Stream *sink)
{
	Ref<HtmlScreen> screen = HtmlScreen::create(text, sink);
	token->project(screen);
}

} // namespace fluxtoki

using namespace fluxtoki;

int main(int argc, char **argv)
{
	String toolName = String(argv[0])->fileName();
	try {
		Ref<Arguments> arguments = Arguments::parse(argc, argv);
		{
			Ref<VariantMap> prototype = VariantMap::create();
			prototype->insert("css", false);
			prototype->insert("language", "");
			prototype->insert("theme", "");
			arguments->validate(prototype);
		}

		VariantMap *options = arguments->options();
		bool cssOption = options->value("css");
		String themeOption = options->value("theme");

		if (cssOption) {
			if (themeOption == "") {
				Ref<StringList> paths = fluxtoki::themeList();
				for (int i = 0; i < paths->size(); ++i) {
					Ref<Theme> theme = Theme::load(paths->at(i));
					writeCss(theme);
				}
			}
		}
	}
	catch (HelpError &) {
		fout(
			"Usage: %% [OPTION]... [FILE]...\n"
			"Generate syntactically colored HTML represenations of source code FILEs.\n"
			"\n"
			"The source code is provided by file or from standard input.\n"
			"For each file a full HTML file (with header and body) is written.\n"
			"The data from standard input is translated into HTML span tags\n"
			"without header or body.\n"
			"\n"
			"Options:\n"
			"  -css       generate/update style sheets\n"
			"  -language  specify source code language\n"
			"  -theme     color theme\n"
		);
		return 1;
	}
	/*catch (Error &ex) {
		ferr() << toolName << ": " << ex.message() << nl;
		return 1;
	}*/

	return 0;
}
