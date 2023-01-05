/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/claim/Report>
#include <cc/claim/NoticeSyntax>
#include <cc/claim/Registry>
#include <cc/DirWalker>
#include <cc/FileInfo>
#include <cc/File>
#include <cc/sha1>

namespace cc::claim {

Report::State::State(const List<String> &dirPaths, const Pattern &works, int worksMinLines):
    dirPaths_{dirPaths},
    works_{works},
    worksMinLines_{worksMinLines}
{
    for (const String &dirPath: dirPaths)
    {
        DirWalker dirWalker{dirPath};
        dirWalker.setIgnoreHidden(true);

        for (const String &path: dirWalker)
        {
            if (FileInfo{path}.type() != FileType::Regular) continue;

            if (!works.match(path.fileName())) continue;

            Notice notice;
            {
                for (int j = 0; j < Registry{}.headerStyleCount(); ++j) {
                    Header header = Registry{}.headerStyleAt(j).scan(path);
                    if (header) {
                        notice = NoticeSyntax{}.readNotice(header);
                        if (notice) break;
                    }
                }
            }

            if (notice) {
                coverage_.insert(path, notice);
                {
                    Bytes digest = cc::sha1(notice.statement().asBytes());

                    Locator pos;
                    if (!coverageByDigest_.find(digest, &pos)) {
                        coverageByDigest_.insert(digest, Coverage{}, &pos);
                        statementByDigest_.insert(digest, notice.statement());
                    }
                    coverageByDigest_.at(pos).value().insert(path, notice);
                }
                {
                    List<Copyright> copyrights = notice.copyrights();
                    for (const Copyright &copyright: copyrights) {
                        String holder = copyright.holder();
                        Locator pos;
                        if (!coverageByHolder_.find(holder, &pos)) {
                            coverageByHolder_.insert(holder, Coverage{}, &pos);
                        }
                        coverageByHolder_.at(pos).value().insert(path, notice);
                    }
                }
            }
            else {
                bool omit = false;
                if (worksMinLines > 1) {
                    String text = File{path}.map();
                    int n = 1;
                    long i = 0;
                    for (; n < worksMinLines && i < text.count(); ++n) {
                        if (!text.find('\n', &i)) break;
                        ++i;
                    }
                    omit = (n < worksMinLines);
                }
                if (!omit) {
                    exposure_.insert(path);
                }
            }
        }
    }
}

} // namespace cc::claim
