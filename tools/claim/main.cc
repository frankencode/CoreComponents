/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/claim/Report>
#include <cc/claim/Registry>
#include <cc/Arguments>
#include <cc/File>
#include <cc/Date>
#include <cc/System>
#include <cc/stdio>

int main(int argc, char *argv[])
{
    using namespace cc::claim;
    using namespace cc;

    String toolName = String{argv[0]}.fileName();

    try {
        Map<String, Variant> options;
        {
            int year = Date{System::now()}.year();
            options.insert("report", false);
            options.insert("strip", false);
            options.insert("insert", false);
            options.insert("focus", "coverage");
            options.insert("holder", "");
            options.insert("year-start", year);
            options.insert("year-end", year);
            options.insert("statement", "");
            options.insert("statement-file", "");
            options.insert("works", "*");
            options.insert("works-min-lines", 10);
        }

        List<String> items = Arguments{argc, argv}.read(&options);

        bool reportOption = options.value("report").to<bool>();
        bool stripOption = options.value("strip").to<bool>();
        bool insertOption = options.value("insert").to<bool>();
        if (!(stripOption || insertOption)) reportOption = true;

        String focus = options.value("focus").to<String>();
        String holder = options.value("holder").to<String>();
        int yearStart = options.value("year-start").to<long>();
        int yearEnd = options.value("year-end").to<long>();
        String statement = options.value("statement").to<String>();
        String statementPath = options.value("statement-file").to<String>();
        Pattern works { options.value("works").to<String>() };
        int worksMinLines = options.value("works-min-lines").to<long>();

        if (statementPath != "") statement = File{statementPath}.map();

        if (items.count() == 0) items << ".";

        Report report{items, works, worksMinLines};

        if (reportOption) {
            if (focus == "coverage") {
                auto coverageByDigest = report.coverageByDigest();
                auto statementByDigest = report.statementByDigest();
                long i = 0;
                for (const auto &item: coverageByDigest) {
                    auto coverage = item.value();
                    Bytes digest = item.key();
                    String statement = statementByDigest.value(digest);
                    fout() << "Statement (" << i + 1 << "):" << nl;
                    fout() << "\"\"\"" << statement << "\"\"\"" << nl;
                    for (const auto &pair: coverage) {
                        fout() << "  " << pair.key() << nl;
                    }
                    fout() << nl;
                    ++i;
                }
            }
            else if (focus == "exposure") {
                for (const auto &e: report.exposure())
                    fout() << e << nl;
                if (report.exposure().count() > 0) return 1;
            }
            else if (focus == "holder") {
                auto coverageByHolder = report.coverageByHolder();
                long i = 0;
                for (const auto &item: coverageByHolder) {
                    auto coverage = item.value();
                    String holder = item.key();
                    fout() << "Copyright " << holder << ":" << nl;
                    for (const auto &pair: coverage) {
                        Notice notice = pair.value();
                        Format line{IoStream::output()};
                        line << "  " << pair.key() << " (";
                        for (const Copyright &copyright: notice.copyrights()) {
                            if (copyright.holder() == holder) {
                                if (copyright.yearStart() < copyright.yearEnd())
                                    line << copyright.yearStart() << "-" << copyright.yearEnd();
                                else
                                    line << copyright.yearStart();
                                break;
                            }
                        }
                        line << ")" << nl << flush;
                    }
                    fout() << nl;
                    ++i;
                }
            }
        }

        if (stripOption) {
            auto coverage = report.coverageByHolder().value(holder);
            if (!coverage) return 0;
            for (const auto &pair: coverage) {
                String path = pair.key();
                Notice notice = pair.value();
                bool otherHolder = false;
                if (holder != "") {
                    for (const auto &copyright: notice.copyrights()) {
                        if (copyright.holder() != holder) {
                            otherHolder = true;
                            break;
                        }
                    }
                }
                if (otherHolder) continue;
                Token token = notice.header().token();
                File file{path, FileOpen::ReadWrite};
                String text = file.map();
                String newText = Format{} << text.copy(0, token.i0()) << text.copy(token.i1(), text.count());
                file.seek(0);
                file.truncate(0);
                file.write(newText);
            }
        }

        if (insertOption) {
            List<Copyright> copyrights;
            copyrights.append(Copyright{holder, yearStart, yearEnd});
            Notice notice{copyrights, statement};
            for (const String &path: report.exposure()) {
                File file{path, FileOpen::ReadWrite};
                String text = file.map();
                HeaderStyle style;
                if (Registry{}.detectHeaderStyle(path, text, &style)) {
                    int magicCount = style.magicCount(text);
                    String magic;
                    if (magicCount > 0) {
                        magic = text.copy(0, magicCount);
                        text = text.copy(magicCount, text.count());
                    }
                    text = magic + style.str(notice) + text.leadingTrimmed();
                    file.seek(0);
                    file.truncate(0);
                    file.write(text);
                }
            }
        }
    }
    catch (HelpRequest &) {
        fout(
            "Usage: %% [OPTION]... [DIR]...\n"
            "Find and update copyright statements.\n"
            "\n"
            "Options:\n"
            "  -report                 generate report with given focus (see -focus)\n"
            "  -strip                  remove all copyright headers of giving copyright holder (see -holder)\n"
            "  -insert                 insert missing copyright headers (see -holder, -year-start, -statement-file)\n"
            "  -focus=[STRING]         'coverage', 'exposure' or 'holder'\n"
            "  -holder=[STRING]        exact name of the license holder\n"
            "  -year-start=[INT]       first year of creation\n"
            "  -year-end=[INT]         last year of creation\n"
            "  -statement=[STRING]     copyright statement\n"
            "  -statement-file=[FILE]  file containing copyright statement\n"
            "  -works=[PATTERN]        file name pattern of copyright protected works ('*' by default)\n"
            "  -works-min-lines=[INT]  minimum number of lines of copyright protected works (10 by default)\n"
        ) << toolName;

        return 1;
    }
    catch (Exception &ex) {
        ferr() << toolName << ": " << ex.message() << nl;
        return 1;
    }

    return 0;
}
