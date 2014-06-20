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
#include <flux/toki/Theme.h>
#include <flux/toki/Language.h>
#include <flux/toki/Registry.h>
#include "HtmlScreen.h"

using namespace flux;
using namespace flux::toki;

String cssPath(String themeName)
{
	return themeName + ".css";
}

int main(int argc, char **argv)
{
	String toolName = String(argv[0])->fileName();
	try {
		Ref<Arguments> arguments = Arguments::parse(argc, argv);
		{
			Ref<VariantMap> prototype = VariantMap::create();
			prototype->insert("css", false);
			prototype->insert("theme", "");
			prototype->insert("language", "");
			prototype->insert("list-languages", false);
			arguments->validate(prototype);
		}

		VariantMap *options = arguments->options();
		StringList *items = arguments->items();
		bool cssOption = options->value("css");
		String themeOption = options->value("theme");
		String languageOption = options->value("language");

		if (options->value("list-languages")) {
			for (int i = 0; i < registry()->languageCount(); ++i)
				fout() << registry()->languageAt(i)->name() << nl;
			return 0;
		}

		if (cssOption) {
			Ref<StringList> paths;
			if (themeOption == "") paths = flux::toki::themeList();
			else paths = StringList::create() << themeOption;
			for (int i = 0; i < paths->count(); ++i) {
				Ref<Theme> theme = Theme::load(paths->at(i));
				Ref<File> file = File::open(cssPath(theme->name()), File::Create|File::Truncate|File::WriteOnly);
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
		}

		Language *defaultLanguage = 0;
		if (languageOption != "") {
			if (!registry()->lookupLanguageByName(languageOption, &defaultLanguage))
				throw UsageError(Format("Language \"%%\" is not supported") << languageOption);
		}

		if (items->count() == 0) {
			Language *language = defaultLanguage;
			if (!language)
				throw UsageError("Please specify language (use -language option)");
			String text = in()->readAll();
			Ref<SyntaxState> state = language->highlightingSyntax()->match(text, 0);
			ferr() << language->highlightingSyntax()->name() << "," << state->i0() << "," << state->i1() << nl;
			if (!state->valid()) throw SyntaxError(text, state, "<standard input>");
			state->rootToken()->project(HtmlScreen::create(text, out()));
		}
		else {
			bool verbose = out()->isatty();
			for (int i = 0; i < items->count(); ++i) {
				String path = items->at(i);
				String text = File::open(path)->map();
				Language *language = defaultLanguage;
				if (!language) {
					if (!registry()->detectLanguage(path, text, &language))
						throw UsageError(Format("%%: Failed to detect language (use -language option)") << path);
				}
				Ref<SyntaxState> state = language->highlightingSyntax()->match(text, 0);
				if (!state->valid()) throw SyntaxError(text, state, path);
				if (themeOption == "") themeOption = "ClassicWhite";
				String htmlPath = path->fileName()->replace(".", "_") + ".html";
				if (verbose) fout() << htmlPath << nl;
				Ref<File> htmlFile = File::open(htmlPath, File::Create|File::Truncate|File::WriteOnly);
				Format(htmlFile) <<
					"<!DOCTYPE HTML>\n"
					"<html>\n"
					"<head>\n"
					"<meta charset=\"UTF-8\">\n"
					"<link rel=\"stylesheet\" href=\"" << cssPath(themeOption) << "\" type=\"text/css\">"
					"</head>\n"
					"<body>\n"
					"<pre>\n";
				state->rootToken()->project(HtmlScreen::create(text, htmlFile));
				Format(htmlFile) <<
					"</pre>\n"
					"</body>\n"
					"</html>\n";
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
			"  -css             generate/update style sheets\n"
			"  -language        specify source code language\n"
			"  -theme           color theme\n"
			"  -list-languages  list all available languages\n"
		) << toolName;
	}
	catch (Exception &ex) {
		ferr() << toolName << ": " << ex.message() << nl;
		return 1;
	}

	return 0;
}
