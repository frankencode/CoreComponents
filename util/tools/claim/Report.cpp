/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/DirWalker.h>
#include <flux/File.h>
#include <flux/crypto/Sha1.h>
#include "Registry.h"
#include "NoticeParser.h"
#include "Report.h"

namespace fluxclaim
{

Ref<Report> Report::create(StringList *dirPaths, Pattern works)
{
	return new Report(dirPaths, works);
}

Report::Report(StringList *dirPaths, Pattern works)
	: dirPaths_(dirPaths),
	  works_(works),
	  coverage_(Coverage::create()),
	  exposure_(Exposure::create()),
	  coverageByDigest_(CoverageByDigest::create()),
	  coverageByHolder_(CoverageByHolder::create()),
	  statementByDigest_(StatementByDigest::create())
{
	for (int i = 0; i < dirPaths->count(); ++i) {
		String dirPath = dirPaths->at(i)->canonicalPath();
		Ref<DirWalker> dirWalker = DirWalker::open(dirPath);
		dirWalker->setIgnoreHidden(true);
		for (String path; dirWalker->read(&path);) {
			if (FileStatus::read(path)->type() != File::Regular) continue;
			if (!works->match(path->fileName())->valid()) continue;
			Ref<Notice> notice;
			{
				const HeaderScannerList *headerScanners = registry()->headerScanners();
				for (int j = 0; j < headerScanners->count(); ++j) {
					Ref<Header> header = headerScanners->at(j)->scanHeader(path);
					if (header) {
						notice = noticeParser()->readNotice(header);
						if (notice) break;
					}
				}
			}
			if (notice) {
				coverage_->insert(path, notice);
				{
					String digest = flux::crypto::sha1(notice->statement());
					Ref<Coverage> coverage;
					if (!coverageByDigest_->lookup(digest, &coverage)) {
						coverageByDigest_->insert(digest, coverage = Coverage::create());
						statementByDigest_->insert(digest, notice->statement());
					}
					coverage->insert(path, notice);
				}
				{
					CopyrightList *copyrights = notice->copyrights();
					for (int j = 0; j < copyrights->count(); ++j) {
						String holder = copyrights->at(j)->holder();
						Ref<Coverage> coverage;
						if (!coverageByHolder_->lookup(holder, &coverage))
							coverageByHolder_->insert(holder, coverage = Coverage::create());
						coverage->insert(path, notice);
					}
				}
			}
			else {
				exposure_->insert(path);
			}
		}
	}
}

} // namespace fluxclaim