/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/File>
#include <cc/Arguments>
#include <cc/toki/Theme>
#include <cc/toki/Language>
#include <cc/toki/Registry>
#include <cc/toki/HtmlScreen>

using namespace cc;
using namespace cc::toki;

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
            prototype->insert("verbose", stdOut()->isatty());
            prototype->insert("css", false);
            prototype->insert("theme", "");
            prototype->insert("language", "");
            prototype->insert("list-languages", false);
            arguments->validate(prototype);
        }

        const VariantMap *options = arguments->options();
        const StringList *items = arguments->items();
        bool verbose = options->value("verbose");
        bool cssOption = options->value("css");
        String themeOption = options->value("theme");
        String languageOption = options->value("language");

        if (options->value("list-languages")) {
            for (int i = 0; i < registry()->languageCount(); ++i) {
                Language *language = registry()->languageAt(i);
                fout() << language->name() << ": \"" << language->displayName() << "\"" << nl;
            }
            return 0;
        }

        if (cssOption) {
            Ref<StringList> paths;
            if (themeOption == "") paths = themeList();
            else paths = StringList::create() << themeOption;
            for (int i = 0; i < paths->count(); ++i) {
                Ref<Theme> theme = Theme::load(paths->at(i));
                Ref<File> file = File::open(cssPath(theme->name()), OpenMode::Create|OpenMode::Truncate|OpenMode::WriteOnly);
                if (verbose) fout() << file->path() << nl;
                HtmlScreen::writeCss(theme, file);
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
            String text = stdIn()->readAll();
            Ref<SyntaxState> state = language->highlightingSyntax()->match(text, 0);
            ferr() << language->highlightingSyntax()->name() << "," << state->i0() << "," << state->i1() << nl;
            if (!state->valid()) throw SyntaxError(text, state, "<standard input>");
            state->rootToken()->project(HtmlScreen::create(text, stdOut()));
        }
        else {
            for (int i = 0; i < items->count(); ++i) {
                String path = items->at(i);
                String text = File::open(path)->map();

                Language *language = defaultLanguage;
                if (!language) {
                    if (!registry()->detectLanguage(path, text, &language))
                        throw UsageError(Format("%%: Failed to detect language (use -language option)") << path);
                }
                Ref<SyntaxState> state;
                while (true) {
                    state = language->highlightingSyntax()->match(text);
                    if (!state->valid()) {
                        ferr() << SyntaxError(text, state, path) << nl;
                        registry()->lookupLanguageByName("plaintext", &language);
                        continue;
                    }
                    break;
                }

                if (themeOption == "") themeOption = "ClassicWhite";
                String htmlPath = path->fileName()->replace(".", "_") + ".html";
                if (verbose) fout() << htmlPath << nl;
                Ref<File> htmlFile = File::open(htmlPath, OpenMode::Create|OpenMode::Truncate|OpenMode::WriteOnly);
                Format sink(htmlFile);
                sink <<
                    "<!DOCTYPE HTML>\n"
                    "<html>\n"
                    "<head>\n"
                    "<meta charset=\"UTF-8\">\n"
                    "<link rel=\"stylesheet\" href=\"" << cssPath(themeOption) << "\" type=\"text/css\">"
                    "</head>\n"
                    "<body>\n";
                state->rootToken()->project(HtmlScreen::create(text, sink));
                sink <<
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
            "  -verbose         verbose output\n"
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
