/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Pattern>
#include <cc/DirWalker>
#include <cc/Arguments>
#include <cc/File>
#include <cc/FileStatus>
#include <cc/stdio>

using namespace cc;

class TextMatch {
public:
    TextMatch() = default;

    explicit TextMatch(long ln, long i0, long i1):
        ln_{ln},
        i0_{i0},
        i1_{i1}
    {}

    long ln() const { return ln_; }
    long i0() const { return i0_; }
    long i1() const { return i1_; }

    void moveTo(long ln, long i0, long i1)
    {
        ln_ = ln;
        i0_ = i0;
        i1_ = i1;
    }

private:
    long ln_ { 0 };
    long i0_ { 0 };
    long i1_ { 0 };
};

List<TextMatch> findMatches(const String &text, const Pattern &pattern);
void displayMatch(const String &path, const String &text, const TextMatch &match);
String replaceMatches(const String &text, List<TextMatch> &matches, const String &replacement);

int main(int argc, char *argv[])
{
    String toolName = String{argv[0]}.fileName();

    try {
        Map<String, Variant> options;
        options.insert("path", "");
        options.insert("name", "");
        options.insert("type", "");
        options.insert("depth", -1);
        options.insert("hidden", false);
        options.insert("text", "");
        options.insert("word", "");
        options.insert("ranges", false);
        options.insert("replace", "");
        options.insert("paste", "");
        options.insert("erase", false);

        Arguments arguments{argc, argv};
        List<String> items = arguments.read(&options);

        Pattern pathPattern { options.value("path").to<String>() };
        Pattern namePattern { options.value("name").to<String>() };
        Pattern typePattern { options.value("type").to<String>() };
        long maxDepth = options.value("depth").to<long>();
        bool ignoreHidden = !options.value("hidden").to<bool>();

        Pattern textPattern { options.value("text").to<String>().expanded() };
        if (options.value("word").to<String>() != "") {
            textPattern = String {
                Format{"(?<!:[a..z]|[A..Z]|[0..9]|_)%%(?>!:[a..z]|[A..Z]|[0..9]|_)"}
                << options.value("word").to<String>()
            };
        }

        bool rangesOption = options.value("ranges").to<bool>();
        bool replaceOption = false;
        String replacement;
        {
            Variant value;
            if (arguments.options().lookup("replace", &value)) {
                replaceOption = true;
                replacement = value.to<String>();
            }
        }
        if (options.value("paste").to<String>() != "") {
            // if (replaceOption == true) // FIXME: multiple conflicting replacement options
            replaceOption = true;
            replacement = File{options.value("paste").to<String>()}.map();
        }
        if (options.value("erase")) {
            // if (replaceOption == true) // FIXME: multiple conflicting replacement options
            replaceOption = true;
            replacement = "";
        }

        if (items.count() == 0) items << ".";

        for (const String &item: items)
        {
            String dirPath = item.canonicalPath();
            DirWalker dirWalker{dirPath};
            dirWalker.setMaxDepth(maxDepth);
            dirWalker.setIgnoreHidden(ignoreHidden);

            for (const String &path: dirWalker)
            {
                if (pathPattern.text() != "") {
                    if (!pathPattern.match(path)) continue;
                }
                if (namePattern.text() != "") {
                    if (!namePattern.match(path.fileName())) continue;
                }
                if (typePattern.text() != "") {
                    bool shortMode = (typePattern.matchLength() == 1);
                    String typeString;
                    switch (FileStatus{path, false}.type()) {
                        case FileType::Regular    : typeString = shortMode ? "r" : "regular file"; break;
                        case FileType::Directory  : typeString = shortMode ? "d" : "directory"; break;
                        case FileType::Symlink    : typeString = shortMode ? "l" : "symlink"; break;
                        case FileType::CharDevice : typeString = shortMode ? "c" : "character device"; break;
                        case FileType::BlockDevice: typeString = shortMode ? "b" : "block device"; break;
                        case FileType::Fifo       : typeString = shortMode ? "f" : "fifo"; break;
                        case FileType::Socket     : typeString = shortMode ? "s" : "socket"; break;
                        case FileType::Undefined  : typeString = shortMode ? "?" : "undefined"; break;
                    };
                    if (!typePattern.findIn(typeString)) continue;
                }
                if (textPattern.text() != "") {
                    if (FileStatus{path}.type() != FileType::Regular)
                        continue;

                    String text = File{path}.map();
                    List<TextMatch> matches = findMatches(text, textPattern);

                    if (replaceOption && matches.count() > 0) {
                        File file{path, FileOpen::ReadWrite};
                        text = replaceMatches(text, matches, replacement);
                        file.truncate(0);
                        file.write(text);
                    }
                    for (const TextMatch &match: matches) {
                        if (rangesOption)
                            fout("%%:%%:%%..%%\n") << path << match.ln() << match.i0() << match.i1();
                        else
                            displayMatch(path, text, match);
                    }
                    continue;
                }
                fout() << path << nl;
            }
        }
    }
    catch (HelpRequest &) {
        fout(
            "Usage: %% [OPTION]... [DIR]...\n"
            "Find (and replace) recursively descending into each [DIR]\n"
            "\n"
            "Options:\n"
            "  -path     file path pattern\n"
            "  -name     file name pattern\n"
            "  -type     file type pattern\n"
            "              r .. regular file\n"
            "              d .. directory\n"
            "              l .. symbolic link\n"
            "              c .. character device\n"
            "              b .. block device\n"
            "              f .. fifo\n"
            "              s .. socket\n"
            "  -depth    maximum depth of recursive directory search\n"
            "  -hidden   also search hidden files and directories\n"
            "  -text     text search pattern\n"
            "  -word     word search pattern\n"
            "  -ranges   show line and byte range for each match\n"
            "  -replace  replace matches by given text\n"
            "  -paste    paste replacement from file\n"
            "  -erase    replace matches by empty String\n"
        ) << toolName;
        return 1;
    }
    catch (SystemResourceError &ex) {
        ferr() << toolName << ": " << ex << nl;
    }
    catch (TextError &ex) {
        ferr() << toolName << ": " << ex << nl;
    }
    #ifdef NDEBUG
    catch (Exception &ex) {
        ferr() << toolName << ": " << ex << nl;
        return 1;
    }
    #endif

    return 0;
}

List<TextMatch> findMatches(const String &text, const Pattern &pattern)
{
    List<TextMatch> matches;
    long ln = 1;
    for (long i = 0; i < text.count();) {
        Range range = pattern.findIn(text, &i);
        if (!range) break;
        long i0 = range[0];
        long i1 = range[1];
        for (;i < i0; ++i) {
            if (text.at(i) == '\n') ++ln;
        }
        matches.append(TextMatch{ln, i0, i1});
        for (;i < i1; ++i) {
            if (text.at(i) == '\n') ++ln;
        }
        if (i0 == i1) ++i;
    }
    return matches;
}

void displayMatch(const String &path, const String &text, const TextMatch &match)
{
    long ln = match.ln();
    long i0 = match.i0();
    long i1 = match.i1();

    long j0 = i0;
    for (;j0 > 0; --j0) {
        if (text.at(j0 - 1) == '\n')
            break;
    }

    fout("%%:") << path;
    if (i0 == i1) {
        long j1 = i0;
        text.find('\n', &j1);
        fout("%%: %%\n") << ln << text.copy(j0, j1);
        return;
    }

    bool multiline = text.copy(i0, i1).contains('\n');
    if (multiline) fout() << nl;

    for (long j1 = j0; j0 < i1; j0 = j1) {
        for (;j1 < text.count(); ++j1) {
            if (text.at(j1) == '\n') break;
        }
        Format line{IoStream::output()};
        line << ln << ": ";
        int k0 = j0, k1 = j1;
        if (j0 <= i0 && i0 < j1) k0 = i0;
        if (j0 < i1 && i1 < j1) k1 = i1;
        if (j0 < k0) line << text.copy(j0, k0);
        if (k0 < k1) line << "\033[7m" << text.copy(k0, k1) << "\033[m";
        if (k1 < j1) line << text.copy(k1, j1);
        line << nl;
        ++ln;
        ++j1;
    }
}

String replaceMatches(const String &text, List<TextMatch> &matches, const String &replacement)
{
    List<String> fragments;
    long fi0 = 0; // begin of fragment
    long si = 0, sl = 0; // index and line shift
    long nr = replacement.count('\n');
    for (TextMatch &match: matches) {
        fragments.append(text.copy(fi0, match.i0()));
        fi0 = match.i1();
        long i0s = match.i0() + si;
        si += replacement.count() - (match.i1() - match.i0());
        sl += nr - text.copy(match.i0(), match.i1()).count('\n');
        match.moveTo(match.ln() + sl, i0s, i0s + replacement.count());
    }
    fragments.append(text.copy(fi0, text.count()));
    return fragments.join(replacement);
}
