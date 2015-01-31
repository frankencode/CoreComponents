/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/DirWalker>
#include <flux/File>
#include <flux/crypto/Sha1>
#include "Registry.h"
#include "NoticeParser.h"
#include "Report.h"

namespace fluxclaim {

Ref<Report> Report::create(StringList *dirPaths, Pattern works, int worksMinLines)
{
    return new Report(dirPaths, works, worksMinLines);
}

Report::Report(StringList *dirPaths, Pattern works, int worksMinLines)
    : dirPaths_(dirPaths),
      works_(works),
      worksMinLines_(worksMinLines),
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
                for (int j = 0; j < registry()->headerStyleCount(); ++j) {
                    Ref<Header> header = registry()->headerStyleAt(j)->scan(path);
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
                bool omit = false;
                if (worksMinLines > 1) {
                    String text = File::open(path)->map();
                    int i = -1, n = 1;
                    for (; n < worksMinLines; ++n) {
                        i = text->find('\n', i + 1);
                        if (i == text->count()) break;
                    }
                    omit = (n < worksMinLines);
                }
                if (!omit)
                    exposure_->insert(path);
            }
        }
    }
}

} // namespace fluxclaim
