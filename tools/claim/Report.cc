/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "Report.h"
#include "NoticeParser.h"
#include "Registry.h"
#include <cc/crypto/Sha1Sink>
#include <cc/DirWalker>
#include <cc/File>

namespace ccclaim {

Ref<Report> Report::create(const StringList &dirPaths, Pattern works, int worksMinLines)
{
    return new Report{dirPaths, works, worksMinLines};
}

Report::Report(const StringList &dirPaths, Pattern works, int worksMinLines):
    dirPaths_{dirPaths},
    works_{works},
    worksMinLines_{worksMinLines}
{
    for (int i = 0; i < dirPaths->count(); ++i) {
        String dirPath = dirPaths->at(i)->canonicalPath();
        Ref<DirWalker> dirWalker = DirWalker::open(dirPath);
        dirWalker->setIgnoreHidden(true);
        for (String path; dirWalker->read(&path);) {
            if (FileStatus::read(path)->type() != FileType::Regular) continue;
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
                    String digest = cc::crypto::sha1(notice->statement());
                    MapValue<String, Coverage>::Iterator it;
                    if (!coverageByDigest_->find(digest, &it)) {
                        coverageByDigest_->insert(digest, Coverage{}, &it);
                        statementByDigest_->insert(digest, notice->statement());
                    }
                    it->value()->insert(path, notice);
                }
                {
                    CopyrightList *copyrights = notice->copyrights();
                    for (int j = 0; j < copyrights->count(); ++j) {
                        String holder = copyrights->at(j)->holder();
                        MapValue<String, Coverage>::Iterator it;
                        if (!coverageByHolder_->find(holder, &it))
                            coverageByHolder_->insert(holder, Coverage{}, &it);
                        it->value()->insert(path, notice);
                    }
                }
            }
            else {
                bool omit = false;
                if (worksMinLines > 1) {
                    String text = File::open(path)->map();
                    int i = -1, n = 1;
                    for (; n < worksMinLines; ++n) {
                        i = text->scan('\n', i + 1);
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

} // namespace ccclaim
