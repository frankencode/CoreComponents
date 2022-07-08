/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/toki/Theme>
#include <cc/toki/Language>
#include <cc/toki/Registry>
#include <cc/toki/HtmlScreen>
#include <cc/File>
#include <cc/Arguments>
#include <cc/stdio>

int main(int argc, char *argv[])
{
    using namespace cc;
    using namespace cc::toki;

    String toolName = String{argv[0]}.fileName();
    try {
        Map<String, Variant> options;
        options.insert("verbose", IoStream::output().isInteractive());
        options.insert("theme", "");
        options.insert("language", "");
        options.insert("list-languages", false);

        List<String> items = Arguments{argc, argv}.read(&options);

        if (options.value("list-languages").to<bool>()) {
            for (long i = 0, n = Registry{}.languageCount(); i < n; ++i) {
                Language language = Registry{}.languageAt(i);
                fout() << language.name() << ": \"" << language.displayName() << "\"" << nl;
            }
            return 0;
        }

        bool verbose = options.value("verbose").to<bool>();
        String themeName = options.value("theme").to<String>();
        String languageName = options.value("language").to<String>();

        Language defaultLanguage;
        if (languageName != "") {
            if (!Registry{}.lookupLanguageByName(languageName, &defaultLanguage)) {
                throw UsageError{Format{"Language \"%%\" is not supported"} << languageName};
            }
        }

        if (themeName == "") themeName = "ClassicWhite";
        Theme theme{themeName};

        if (items.count() == 0) {
            if (!defaultLanguage) {
                throw UsageError{"Please specify language (use -language option)"};
            }
            Language language = defaultLanguage;
            String text = IoStream::input().readAll();
            Token token = language.highlightingSyntax().match(text);
            ferr() << language.name() << "," << token[0] << "," << token[1] << nl;
            if (!token) throw TextError{text, token[0]};
            token.project(HtmlScreen{text, theme, fout()});
        }
        else {
            for (const String &path: items)
            {
                String text = File{path}.map();

                Language language = defaultLanguage;
                if (!language) {
                    if (!Registry{}.detectLanguage(path, text, &language))
                        throw UsageError{Format{"%%: Failed to detect language (use -language option)"} << path};
                }

                Token token = language.highlightingSyntax().match(text);
                if (!token) throw TextError{text, token[0]};

                String htmlPath = path + ".html";

                if (verbose) ferr() << htmlPath << nl;

                Format sink{File{htmlPath, FileOpen::Overwrite}};
                sink <<
                    "<!DOCTYPE HTML>\n"
                    "<html>\n"
                    "<head>\n"
                    "<meta charset=\"UTF-8\">\n"
                    "</head>\n"
                    "<body>\n";

                token.project(HtmlScreen{text, theme, sink});

                sink <<
                    "</body>\n"
                    "</html>\n";
            }
        }
    }
    catch (HelpRequest &)
    {
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
            "  -language        specify source code language\n"
            "  -theme           color theme\n"
            "  -list-languages  list all available languages\n"
        ) << toolName;
    }

    return 0;
}
