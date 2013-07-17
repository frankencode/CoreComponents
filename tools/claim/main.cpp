/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <fkit/stdio.h>
#include <fkit/Config.h>
#include "Report.h"

using namespace fkit;
using namespace fclaim;

int main(int argc, char **argv)
{
	Ref<Config> config;
	try {
		config = Config::read(argc, argv);
	}
	catch (HelpException &) {
		fout(
			"Usage: %% [OPTION]... [DIR]...\n"
			"Find and update copyright statements.\n"
			"\n"
			"Options:\n"
			"  -report=[STRING]  report on 'coverage', 'exposure' or 'holder'\n"
			"  -strip=[STRING]   remove all copyright headers of given copyright holder\n"
			"  -insert=[FILE]    insert missing copyright headers from file\n"
			"  -works=[PATTERN]  file name pattern of copyright protected works\n"
		) << String(argv[0])->fileName();

		return 0;
	}

	String focus;
	String holder;
	String headerPath;
	bool reportOption = config->lookup("report", &focus);
	bool stripOption = config->lookup("strip", &holder);
	bool insertOption = config->lookup("insert", &headerPath);
	Pattern works = config->value("works", "*.*");

	if (!(reportOption || stripOption || insertOption)) {
		reportOption = true;
		focus = "coverage";
	}

	StringList *dirPaths = config->arguments();
	if (dirPaths->length() == 0) dirPaths->append(".");

	Ref<Report> report = Report::create(dirPaths, works);

	if (reportOption) {
		if (focus == "coverage") {
			CoverageByDigest *coverageByDigest = report->coverageByDigest();
			StatementByDigest *statementByDigest = report->statementByDigest();
			for (int i = 0; i < coverageByDigest->size(); ++i) {
				Coverage *coverage = coverageByDigest->valueAt(i);
				String digest = coverageByDigest->keyAt(i);
				String statement = statementByDigest->value(digest);
				fout() << "Statement (" << i + 1 << "):" << nl;
				fout() << "\"\"\"" << statement << "\"\"\"" << nl;
				for (int j = 0; j < coverage->size(); ++j)
					fout() << "  " << coverage->keyAt(j) << nl;
				fout() << nl;
			}
		}
		else if (focus == "exposure") {
			Exposure *exposure = report->exposure();
			for (int i = 0; i < exposure->size(); ++i)
				fout() << exposure->at(i) << nl;
			if (exposure->size() > 0) return 1;
		}
		else if (focus == "holder") {
			CoverageByHolder *coverageByHolder = report->coverageByHolder();
			for (int i = 0; i < coverageByHolder->size(); ++i) {
				Coverage *coverage = coverageByHolder->valueAt(i);
				String holder = coverageByHolder->keyAt(i);
				fout() << "Copyright " << holder << ":" << nl;
				for (int j = 0; j < coverage->size(); ++j) {
					Notice *notice = coverage->valueAt(j);
					Format line(out());
					line << coverage->keyAt(j) << " (";
					CopyrightList *copyrights = notice->copyrights();
					for (int k = 0; k < copyrights->length(); ++k) {
						Copyright *copyright = copyrights->at(k);
						if (copyright->holder() == holder) {
							if (copyright->yearStart() < copyright->yearEnd())
								line << copyright->yearStart() << "-" << copyright->yearEnd();
							else
								line << copyright->yearStart();
							break;
						}
					}
					line << ")" << nl << flush;
				}
				fout() << nl;
			}
		}
	}
	if (stripOption) {
		Coverage *coverage = report->coverageByHolder()->value(holder);
		if (!coverage) return 0;
		for (int i = 0; i < coverage->size(); ++i) {
			String path = coverage->keyAt(i);
			Notice *notice = coverage->valueAt(i);
			CopyrightList *copyrights = notice->copyrights();
			for (int j = 0; j < copyrights->length(); ++j)
				if (copyrights->at(j)->holder() != holder) continue;
			Token *token = notice->header()->token();
			Ref<File> file = File::open(path, File::Read|File::Write);
			String text = file->map();
			String newText = Format() << text->copy(0, token->i0()) << text->copy(token->i1(), text->length());
			file->seek(0);
			file->truncate(0);
			file->write(newText);
		}
	}
	if (insertOption) {
		String header = File::open(headerPath)->map();
		Exposure *exposure = report->exposure();
		for (int i = 0; i < exposure->size(); ++i) {
			String path = exposure->at(i);
			Ref<File> file = File::open(path, File::Read|File::Write);
			String newText = Format() << header << file->map();
			file->seek(0);
			file->truncate(0);
			file->write(newText);
		}
	}

	return 0;
}
